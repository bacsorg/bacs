#!/bin/bash -eu

source "$(dirname "$0")/travis.sh"

function gen_cmake() (
  mkdir "$1"
  cd "$1"
  shift
  cmake \
      -DENABLE_WEB=OFF \
      -DENABLE_CURL=OFF \
      -DBoost_ADDITIONAL_VERSIONS="1.66.0" \
      -DBOOST_ROOT="$BOOST_ROOT" \
      -DBOOST_INCLUDEDIR="$BOOST_INCLUDEDIR" \
      -DBOOST_LIBRARYDIR="$BOOST_LIBRARYDIR" \
      -DBoost_NO_SYSTEM_PATHS=ON \
      -DProtobuf_INCLUDE_DIR="$HOME_PREFIX/include" \
      -DProtobuf_LIBRARIES="$HOME_PREFIX/lib/libprotobuf.so" \
      -DGRPC_INCLUDE_DIR="$HOME_PREFIX/include" \
      -DGPR_LIBRARY="$HOME_PREFIX/lib/libgpr.so" \
      -DGRPC_LIBRARY="$HOME_PREFIX/lib/libgrpc.so" \
      -DGRPCXX_LIBRARY="$HOME_PREFIX/lib/libgrpc++.so" \
      -DGRPC_CXX_PLUGIN="$HOME_PREFIX/bin/grpc_cpp_plugin" \
      -DGRPC_PYTHON_PLUGIN="$HOME_PREFIX/bin/grpc_python_plugin" \
      .. "$@"
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
  *)
      echo "Invalid BUILD=$BUILD"
      exit 1
    ;;
esac
