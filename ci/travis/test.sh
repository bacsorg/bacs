#!/bin/bash -eu

source "$(dirname "$0")/travis.sh"

function gen_cmake() (
  mkdir "$1"
  cd "$1"
  shift
  cmake .. "$@"
)

case "$BUILD" in
  meson)
      run meson builddir
      run ninja -C builddir -j"$JOBS"
      run ninja -C builddir test
    ;;
  cmake-make)
      run gen_cmake build
      run make -C build -j"$JOBS"
      run make -C build test
    ;;
  cmake-ninja)
      run gen_cmake build -G Ninja
      run ninja -C build -j"$JOBS"
      run ninja -C build test
    ;;
esac
