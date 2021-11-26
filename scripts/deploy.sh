#!/bin/bash -e

source "$(dirname "$0")/lib/root.sh"
source "$(dirname "$0")/lib/cdroot.sh"

if [[ -f "$1/${repo_name}.db" ]]
then
    rm -f "$1/"*
    tar xf "${repo_name}.tar" -C "$1"
else
    echo "Will not override not repository!" >&2
    exit 1
fi
