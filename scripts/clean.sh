#!/bin/bash -e

source "$(dirname "$0")/lib/cdroot.sh"

git submodule foreach git clean -dfx
git submodule foreach rm -rf src pkg
git clean -e '!/chroot/' -dfX
# TODO no safe solution to clean src/*-git yet
