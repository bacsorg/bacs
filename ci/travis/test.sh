#!/bin/bash -eu

source "$(dirname "$0")/travis.sh"

case "$BUILD" in
  meson)
      run meson builddir
      run ninja -C builddir -j"$JOBS"
      run ninja -C builddir test
    ;;
  *)
      echo "Invalid BUILD=$BUILD"
      exit 1
    ;;
esac
