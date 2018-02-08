#!/bin/bash -eu

source "$(dirname "$0")/travis.sh"

python3=/usr/bin/python3.6

# TODO migrate to stable version once it's ready
meson_git='https://github.com/mesonbuild/meson'

ninja_ver='1.8.2'
ninja_bin="http://github.com/ninja-build/ninja/releases/download/v${ninja_ver}/ninja-linux.zip"
ninja_sha256='d2fea9ff33b3ef353161ed906f260d565ca55b8ca0568fa07b1d2cab90a84a07'

boost_ver='1.66.0'
boost_src="https://dl.bintray.com/boostorg/release/${boost_ver}/source/boost_$(echo "$boost_ver" | tr . _).tar.gz"
boost_sha256='bd0df411efd9a585e5a2212275f8762079fed8842264954675a4fddc46cfcf60'
boost_dir="$(basename "$boost_src" .tar.gz)"

turtle_ver='1.3.0'
turtle_src="http://downloads.sourceforge.net/project/turtle/turtle/$turtle_ver/turtle-${turtle_ver}.tar.bz2"
turtle_sha256='1f0a8f7b7862e0f99f3849d60b488b1ce6546f1f7cfeb4d8f6c0261f1e3dcbe0'

botan_ver='2.4.0'
botan_src="https://botan.randombit.net/releases/Botan-${botan_ver}.tgz"
botan_sha256='ed9464e2a5cfee4cd3d9bd7a8f80673b45c8a0718db2181a73f5465a606608a5'
botan_dir="$(basename "$botan_src" .tgz)"

function sha256verify {
  local file="$1"
  local hash="$2"
  echo "$hash $file" | sha256sum -c
}

function install_meson() (
  run git clone "$meson_git"
  cd meson
  run "$python3" setup.py build_scripts --executable="$python3"
  run sudo "$python3" setup.py install --prefix=/usr
)

function install_ninja() (
  run wget "$ninja_bin"
  run sha256verify "$(basename "$ninja_bin")" "$ninja_sha256"
  run unzip "$(basename "$ninja_bin")"
  run sudo install -Dm755 ninja /usr/bin/ninja
)

function install_boost() (
  # TODO cache this
  run wget "$boost_src"
  run sha256verify "$(basename "$boost_src")" "$boost_sha256"
  run tar xzf "$(basename "$boost_src")"
  cd "$boost_dir"
  run ./bootstrap.sh --prefix=/usr
  b2opts=(
      link=shared
      threading=multi
      variant=release
  )
  run ./b2 "${b2opts[@]}" -j"$JOBS"
  # Reduce verbosity of installation
  run sudo ./b2 "${b2opts[@]}" install | egrep -v '^common\.copy'
)

function install_turtle() (
  run wget "$turtle_src"
  run sha256verify "$(basename "$turtle_src")" "$turtle_sha256"
  run sudo tar xjf turtle-1.3.0.tar.bz2 -C /usr include
)

function install_botan() (
  # TODO cache this
  run wget "$botan_src"
  run sha256verify "$(basename "$botan_src")" "$botan_sha256"
  run tar xzf "$(basename "$botan_src")"
  cd "$botan_dir"
  run ./configure.py --prefix=/usr
  run make -j"$JOBS"
  run sudo make install
)

fold install_meson
fold install_ninja
fold install_boost
fold install_turtle
fold install_botan
