#!/bin/bash -e

source "$(dirname "$0")/lib/counter.sh"
source "$(dirname "$0")/lib/root.sh"
source "$(dirname "$0")/lib/makechrootpkg.sh"

make_chroot_pkg
release_repo
