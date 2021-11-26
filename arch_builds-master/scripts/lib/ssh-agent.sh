#!/bin/bash -e

if [[ ! -f "/proc/$SSH_AGENT_PID/stat" ]]
then
    exec ssh-agent "$0" "$@"
fi
