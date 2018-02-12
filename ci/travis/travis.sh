#!/bin/bash -eu

JOBS="${JOBS:-3}"

function fold {
  echo "travis_fold:start:$1"
  "$@"
  echo "travis_fold:end:$1"
}

function run {
  echo "$ $*"
  "$@"
}
