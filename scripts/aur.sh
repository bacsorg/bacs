#!/bin/bash -e

script="$(readlink -f "$0")"
hook="$(dirname "$script")/pre-commit.sh"

if [[ -f .git ]]
then
    module="$(basename "$PWD")"
    url="ssh://aur@aur4.archlinux.org/${module}.git"
    if git remote add aur "$url" &>/dev/null
    then
        echo "Added aur = $url"
    else
        git remote set-url aur "$url"
        echo "Fixed aur = $url"
    fi
    gitdir="$(sed -r 's|^gitdir: (.*)$|\1|' <.git)"
    rm -f "$gitdir/hooks/pre-commit"
    cp -f "$hook" "$gitdir/hooks/pre-commit"
    echo "Added $gitdir/hooks/pre-commit hook"
else
    git submodule foreach "$script"
fi
