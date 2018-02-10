#!/bin/bash -eu

source "$(dirname "$0")/travis.sh"

python3=/usr/bin/python3.6

# TODO migrate to stable version once it's ready
meson_git='https://github.com/mesonbuild/meson'

ninja_ver='1.8.2'
ninja_bin="http://github.com/ninja-build/ninja/releases/download/v${ninja_ver}/ninja-linux.zip"
ninja_fname='ninja.zip'
ninja_sha256='d2fea9ff33b3ef353161ed906f260d565ca55b8ca0568fa07b1d2cab90a84a07'
ninja_cache_check="$HOME_PREFIX/bin/ninja"

boost_ver='1.66.0'
boost_src="https://dl.bintray.com/boostorg/release/${boost_ver}/source/boost_$(echo "$boost_ver" | tr . _).tar.gz"
boost_fname='boost.tar.gz'
boost_sha256='bd0df411efd9a585e5a2212275f8762079fed8842264954675a4fddc46cfcf60'
boost_dir="$(basename "$boost_src" .tar.gz)"
boost_cache_check="$HOME_PREFIX/lib/libboost_filesystem.so.$boost_ver"

turtle_ver='1.3.0'
turtle_src="http://downloads.sourceforge.net/project/turtle/turtle/$turtle_ver/turtle-${turtle_ver}.tar.bz2"
turtle_fname='turtle.tar.bz2'
turtle_sha256='1f0a8f7b7862e0f99f3849d60b488b1ce6546f1f7cfeb4d8f6c0261f1e3dcbe0'
turtle_cache_check="$HOME_PREFIX/include/turtle/version.hpp"

botan_ver='2.4.0'
botan_src="https://botan.randombit.net/releases/Botan-${botan_ver}.tgz"
botan_fname='botan.tgz'
botan_sha256='ed9464e2a5cfee4cd3d9bd7a8f80673b45c8a0718db2181a73f5465a606608a5'
botan_dir="$(basename "$botan_src" .tgz)"
botan_cache_check="$HOME_PREFIX/lib/libbotan-2.so.4.4.0"

protobuf_ver='3.5.1.1'
protobuf_src="https://github.com/google/protobuf/archive/v${protobuf_ver}.tar.gz"
protobuf_fname='protobuf.tar.gz'
protobuf_sha256='56b5d9e1ab2bf4f5736c4cfba9f4981fbc6976246721e7ded5602fbaee6d6869'
protobuf_dir="protobuf-$protobuf_ver"
protobuf_cache_check="$HOME_PREFIX/lib/libprotobuf.so.15.0.1"

cares_ver='1.13.0'
cares_src="https://c-ares.haxx.se/download/c-ares-${cares_ver}.tar.gz"
cares_fname='cares.tar.gz'
cares_sha256='03f708f1b14a26ab26c38abd51137640cb444d3ec72380b21b20f1a8d2861da7'
cares_dir="c-ares-$cares_ver"
cares_cache_check="$HOME_PREFIX/lib/libcares.so.2.2.0"

grpc_ver='1.9.1'
grpc_src="https://github.com/grpc/grpc/archive/v${grpc_ver}.tar.gz"
grpc_fname="grpc.tar.gz"
grpc_sha256='fac236d49fb3e89399b68a5aa944fc69221769bcedd099d47eb6f93e59035c40'
grpc_dir="grpc-$grpc_ver"
grpc_cache_check="$HOME_PREFIX/lib/libgrpc++.so.$grpc_ver"

function fetch {
  local url="$1"
  local output="$2"
  # Timeout is 5min, less than 10min travis-ci timeout
  wget \
      --retry-connrefused \
      --tries=inf \
      --timeout=300 \
      "$url" \
      --output-document="$output"
}

function sha256verify {
  local file="$1"
  local hash="$2"
  echo "$hash $file" | sha256sum -c
}

# Use when file checked is versioned already.
function use_cache {
  local name="$1"
  local check="$2"
  if [[ ! -f $check ]]; then
    echo "Will rebuild $name, $check not found"
    return
  fi
  echo "Will not rebuild $name, found $check"
  exit 0  # assuming caller is a subshell
}

# Use when you need to run custom code to parse version.
function use_cache_ver {
  local name="$1"
  local ver="$2"
  local check="$3"
  shift 3
  local cache_ver="$("$@" | tr -d '[:space:]')"
  if [[ ! -f $check ]]; then
    echo "Will rebuild $name, $check not found"
    return
  fi
  if [[ $ver != $cache_ver ]]; then
    echo "Will rebuild $name, version want $ver, cached $cache_ver"
    return
  fi
  echo "Will not rebuild $name, version matched"
  exit 0
}

function install_dir {
  local src="$1"
  local dst="$2"
  mkdir -p "$dst"
  (cd "$src" && tar cf - .) | (cd "$dst" && tar xf -)
}

function install_meson() (
  run git clone "$meson_git"
  cd meson
  run "$python3" setup.py build_scripts --executable="$python3"
  run "$python3" setup.py install --prefix="$HOME_PREFIX"
)

function install_ninja() (
  use_cache_ver ninja "$ninja_ver" "$ninja_cache_check" \
      "$ninja_cache_check" --version
  run fetch "$ninja_bin" "$ninja_fname"
  run sha256verify "$ninja_fname" "$ninja_sha256"
  run unzip "$ninja_fname"
  run install -Dm755 ninja "$HOME_PREFIX/bin/ninja"
)

function install_boost() (
  use_cache boost "$boost_cache_check"
  run fetch "$boost_src" "$boost_fname"
  run sha256verify "$boost_fname" "$boost_sha256"
  run tar xzf "$boost_fname"
  cd "$boost_dir"
  run ./bootstrap.sh --prefix="$HOME_PREFIX"
  b2opts=(
      link=shared
      threading=multi
      variant=release
  )
  run ./b2 "${b2opts[@]}" -j"$JOBS"
  # Reduce verbosity of installation
  run ./b2 "${b2opts[@]}" install | egrep -v '^common\.copy'
)

function install_turtle() (
  use_cache_ver turtle "$turtle_ver" "$turtle_cache_check" \
      sed -rn 's|^.*MOCK_VERSION +([^ ]+)$|\1|p' "$turtle_cache_check"
  run fetch "$turtle_src" "$turtle_fname"
  run sha256verify "$turtle_fname" "$turtle_sha256"
  run tar xjf "$turtle_fname" -C "$HOME_PREFIX" include
)

function install_botan() (
  use_cache botan "$botan_cache_check"
  run fetch "$botan_src" "$botan_fname"
  run sha256verify "$botan_fname" "$botan_sha256"
  run tar xzf "$botan_fname"
  cd "$botan_dir"
  run ./configure.py --prefix="$HOME_PREFIX"
  run make -j"$JOBS"
  run make install
)

function install_protobuf() (
  use_cache protobuf "$protobuf_cache_check"
  run fetch "$protobuf_src" "$protobuf_fname"
  run sha256verify "$protobuf_fname" "$protobuf_sha256"
  run tar xzf "$protobuf_fname"
  cd "$protobuf_dir"
  run ./autogen.sh
  run ./configure --prefix="$HOME_PREFIX"
  run make -j"$JOBS"
  run make install
)

function install_cares() (
  use_cache cares "$cares_cache_check"
  run fetch "$cares_src" "$cares_fname"
  run sha256verify "$cares_fname" "$cares_sha256"
  run tar xzf "$cares_fname"
  cd "$cares_dir"
  run ./configure --prefix="$HOME_PREFIX"
  run make -j"$JOBS"
  run make install
)

function install_grpc() (
  use_cache grpc "$grpc_cache_check"
  run fetch "$grpc_src" "$grpc_fname"
  run sha256verify "$grpc_fname" "$grpc_sha256"
  run tar xzf "$grpc_fname"
  cd "$grpc_dir"
  sed -r 's|-Werror||g' -i Makefile
  run make -j"$JOBS" prefix="$HOME_PREFIX"
  run install_dir bins/opt "$HOME_PREFIX/bin"
  run install_dir include "$HOME_PREFIX/include"
  run install_dir libs/opt "$HOME_PREFIX/lib"
)

fold install_meson
fold install_ninja
fold install_boost
fold install_turtle
fold install_botan
fold install_protobuf
fold install_cares
fold install_grpc
