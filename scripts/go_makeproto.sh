#!/bin/bash -e

script="$(readlink -f "$0")"
src="$(dirname "$(dirname "$script")")"

function run {
  echo '$' "$@"
  "$@"
}

run find "$src/go" -name '*.pb.go' -exec git rm -f '{}' '+'

proto_includes=(
    bunsan/broker/include
    bacs/common/include
    bacs/problem/include
    bacs/archive/include
)

protos=(`find "${proto_includes[@]}" -name '*.proto'`)
proto_packages=("${protos[@]%/*.proto}")

for dir in "${proto_packages[@]}"; do
  dir_protos=("$dir/"*.proto)
  go_protos=("${dir_protos[@]/%.proto/.pb.go}")
  go_protos=("${go_protos[@]/#*include/go}")
  run protoc "${proto_includes[@]/#/--proto_path=}" \
      "${dir_protos[@]}" \
      --go_out=plugins=grpc:"$src/go"
  run git add "${go_protos[@]}"
done
