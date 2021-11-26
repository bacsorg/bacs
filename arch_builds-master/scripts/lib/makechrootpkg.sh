#!/bin/bash -e

source "$(dirname "$0")/lib/chroot.sh"

export LC_ALL=C

readonly makechrootpkg="$bindir/makechrootpkg"

patch_makechrootpkg()
{
    mkdir -p "$root/scripts/bin"
    sed -r 's|--verifysource|& --skippgpcheck --holdver|' \
        "$(which makechrootpkg)" >"$makechrootpkg"
    chmod +x "$makechrootpkg"
}

backup_pkgbuild()
{
    cp -f PKGBUILD PKGBUILD.bak
}

restore_pkgbuild()
{
    cp -f PKGBUILD.bak PKGBUILD
}

pkg_name_version()
(
    . "$1"
    if [[ -n $epoch ]]
    then
        echo -n "$epoch:"
    fi
    # let pkgrel to be overridden
    pkgrel="$2"
    echo "$pkgver-$pkgrel"
)

raw_make_chroot_pkg()
{
    backup_pkgbuild
    {
        patch_makechrootpkg
        sudo -u "$user" -g "$group" env "SSH_AUTH_SOCK=$SSH_AUTH_SOCK" \
            makepkg --nobuild --nodeps --skippgpcheck
        sed -r "s|^pkgrel=.*$|pkgrel=${reporel}|" -i PKGBUILD
        pkgversion="$(pkg_name_version ./PKGBUILD "$reporel")"
        "$makechrootpkg" -r "$chroot" -- --holdver --skippgpcheck "$@"
    } || {
        ret=$?
        restore_pkgbuild
        return $ret
    }
    restore_pkgbuild
}

make_chroot_pkg()
{
    echo >&2
    echo "[[[ Making package $PWD ]]]" >&2
    echo >&2
    local try
    for try in `seq 1 10`
    do
        if raw_make_chroot_pkg "$@"
        then
            break
        else
            echo "Failed $PWD, retrying $try time in 10 seconds, press Ctrl-C to abort..." >&2
            sleep 10
        fi
    done
    local cpkgname
    for cpkgname in $(. ./PKGBUILD && echo "${pkgname[@]}")
    do
        echo "Adding $cpkgname" >&2
        local arch=""

        for arch in any i686 x86_64
        do
            # pkgversion is set in raw_make_chroot_pkg()
            local spkgfile="$cpkgname-$pkgversion-$arch.pkg.tar.xz"
            if [[ -f $spkgfile ]]
            then
                local pkgfile="$(readlink -f "$spkgfile")"
                echo "$spkgfile exists as $pkgfile" >&2
                break
            else
                echo "$spkgfile does not exist, retrying..." >&2
            fi
        done
        if [[ ! -f $spkgfile ]]
        then
            echo "Unable to find $cpkgname package file" >&2
            return 1
        fi
        add_to_db "$user" "$pkgfile"
        failed=0
        while ! chroot_run "$user" pacman --noconfirm -Sw "$cpkgname"
        do
            add_to_db "$user" "$pkgfile"
            failed=$((failed + 1))
            if (( failed >= 10 ))
            then
                echo "Unable to build ${PWD#$root}" >&2
                return 1
            fi
        done
    done
    echo >&2
    echo "[[[ $PWD make completed ]]]" >&2
}
