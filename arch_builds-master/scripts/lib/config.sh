#!/bin/bash -e

script="$(readlink -f "$0")"
root="$(cd "$(dirname "$(dirname "$script")")" && pwd)"
scriptsdir="$root/scripts"
bindir="$scriptsdir/bin"
libdir="$scriptsdir/lib"

# Aleksey Filippov <sarum9in@gmail.com>
signkey="754CD5CD2FA687AC1B277181E21B9964B5425F2B"

repo_name="breakingkeys"

try_source()
{
    local src
    for src
    do
        if [[ -f $src ]]
        then
            source "$src"
        fi
    done
}

# User settings
try_source "$scriptsdir/etc/config.sh"
