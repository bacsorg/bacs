#!/bin/bash -eu

source "$(dirname "$0")/travis.sh"

run meson builddir
run ninja -C builddir -j"$JOBS"
run ninja -C builddir test
