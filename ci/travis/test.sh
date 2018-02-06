#!/bin/bash -eu

source "$(dirname "$0")/travis.sh"

# FIXME use system version of meson
run python3.6 ./meson/meson.py builddir
run ninja -C builddir -j"$JOBS"
run ninja -C builddir test
