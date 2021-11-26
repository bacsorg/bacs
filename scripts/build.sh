#!/bin/bash -e

source "$(dirname "$0")/lib/root.sh"

package_set="$(basename "$PWD")"
target="$1" # not essential

run_makepkg()
{
    rm -f *.pkg.tar.xz
    makepkg -os --asroot --noconfirm --syncdeps
    chown "$user:$group" -R .
    su "$user" -c 'makepkg -f'
}

install_main()
{
    makepkg -i --asroot --noconfirm
}

install_dep()
{
    pacman -U --asdeps --noconfirm *.pkg.tar.xz
}

if [[ -f PKGBUILD ]]
then
    run_makepkg
    install_dep
else
    for i in `tsort dependencies`
    do
        pushd "$i"
        run_makepkg
        if [[ $i = $target ]] || fgrep "$i" install >/dev/null 2>&1
        then
            install_main
        else
            install_dep
        fi
        popd
        if [[ $i = $target ]]
        then
            break
        fi
    done
fi
