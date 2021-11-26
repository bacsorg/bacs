#!/bin/bash -e

source "$(dirname "$0")/lib/config.sh"

vcs=(bzr git mercurial svn)
languages=(
    go
    php
    python2 python cython cython2
    cargo rust
)

chroot="$root/chroot"

chroot_run()
{
    local chroot_="$chroot/$1"
    shift
    arch-nspawn "$chroot_" "$@"
}

repo_sync()
{
    chroot_run "${1:-root}" pacman -Sy
}

add_to_db()
{
    local prefix="$1"
    local pkgfile="$2"
    local pkgname="$(basename "$pkgfile")"
    local repo_dir="$chroot/$prefix/repo"
    local repo_db="$repo_dir/${repo_name}.db.tar.gz"
    local repo_sig="$repo_dir/${repo_db}.sig"
    local pkgdest="$repo_dir/$pkgname"
    mkdir -p "$repo_dir"
    chown "$user:$group" "$repo_dir"
    sudo -u "$user" -g "$group" cp -vf "$pkgfile" "$pkgdest"
    sign_file "$pkgdest"
    sudo -u "$user" -g "$group" repo-add "$repo_db" "$pkgdest"
    rm -f "$repo_sig"
    sign_file "$repo_db"
    repo_sync "$prefix"
}

init_keyring()
{
    pacman-key --config="$chroot/root/etc/pacman.conf" --recv-keys "$signkey"
    pacman-key --config="$chroot/root/etc/pacman.conf" --lsign "$signkey"
}

sign_file()
{
    local file="$1"
    while true
    do
        rm -f "${file}.sig"
        if ! sudo -u "$user" -g "$group" env "GPG_TTY=$GPG_TTY" \
            gpg --detach-sign \
                --local-user "$signkey" \
                --use-agent \
                --no-armor \
                "$file"
        then
            local readstub
            echo "gpg --detach-sign failed, press enter to retry" >&2
            read readstub
        else
            break
        fi
    done
}

new_chroot()
{
    mkdir -p "$chroot"
    mkarchroot "$chroot/root" base base-devel sudo "${vcs[@]}" "${languages[@]}"
    local mflags="$(sed -rn 's|^MAKEFLAGS=(".*")$|\1|p' /etc/makepkg.conf)"
    if [[ -n $mflags ]]
    then
        sed -ri "s|^.*(MAKEFLAGS=).*$|\1$mflags|" "$chroot/root/etc/makepkg.conf"
    fi
    sed -ri 's|^CheckSpace|#&|' "$chroot/root/etc/pacman.conf"
    sed -ri 's|^(SigLevel[[:space:]]*=).*$|\1 TrustedOnly|' "$chroot/root/etc/pacman.conf"
    sed -ri 's|^.*REPOSITORIES.*$|&\n[repo]\nServer = file:///repo\n|' "$chroot/root/etc/pacman.conf"
    sed -ri "s|^\[repo\]$|[$repo_name]|" "$chroot/root/etc/pacman.conf"
    init_keyring
    pushd "$root/empty"
    rm -f *.pkg.tar.xz
    sudo -u "$user" -g "$group" makepkg -f
    local pkgname="$(ls *.pkg.tar.xz)"
    add_to_db root "$pkgname"
    popd &>/dev/null
}

update_chroot()
{
    chroot_run root pacman -Suy --noconfirm
}

make_chroot()
{
    pacman --noconfirm -Suwy
    if [[ -d $chroot/root ]]
    then
        if ! update_chroot
        then
            rm -rf "$chroot"
            new_chroot
        fi
    else
        new_chroot
    fi
}
