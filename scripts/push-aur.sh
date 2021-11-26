#!/bin/bash -e

script="$(readlink -f "$0")"
source "$(dirname "$0")/lib/ssh-agent.sh"

if [[ -f .git ]]
then
    git push aur HEAD:master
else
    ssh-add ~/.ssh/id_rsa-aur
    git submodule foreach "$script"
fi
