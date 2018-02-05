#!/bin/bash -eu

function fold {
  echo "travis_fold:start:$*"
  "$1"
  echo "travis_fold:end:$*"
}

function run {
  echo "$ $*"
  "$@"
}
