#!/bin/bash

CHALLENGE_NAME="secure-file-scanner-v2"

# set to docker if presnt, otherwise podman
CONTAINER_RUNTIME="podman"
if command -v docker &>/dev/null; then
    CONTAINER_RUNTIME="docker"
fi

set -e

FLAG=$(cat flag.txt)
"$CONTAINER_RUNTIME" build --file Dockerfile --no-cache --build-arg FLAG="$FLAG" --tag wpctf2024-challenges:"$CHALLENGE_NAME" .

# Extract the binary from the built container so the same version can be given to the participants
tmp_container_id=$($CONTAINER_RUNTIME create wpctf2024-challenges:$CHALLENGE_NAME)
$CONTAINER_RUNTIME cp "$tmp_container_id":/app/scanner-v2 .

$CONTAINER_RUNTIME rm "$tmp_container_id"

if [ -f "$CHALLENGE_NAME".zip ]; then
    rm "$CHALLENGE_NAME".zip
fi

zip "$CHALLENGE_NAME".zip scanner-v2
