#!/bin/bash -eu

JOBS=3

function fold {
  echo "travis_fold:start:$*"
  "$1"
  echo "travis_fold:end:$*"
}

function run {
  echo "$ $*"
  "$@"
}
