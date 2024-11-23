#!/bin/bash

CHALLENGE_NAME="strange-boltmat"
SERVER_IMAGE="wpctf2024-challenges:$CHALLENGE_NAME-server"
BUILD_IMAGE="wpctf2024-challenges:$CHALLENGE_NAME-build"

# set to docker if present, otherwise podman
CONTAINER_RUNTIME="podman"
if command -v docker &>/dev/null; then
    CONTAINER_RUNTIME="docker"
fi

set -e

if [ -z "$1" ]; then
    echo "Usage: $0 <encoded ip>"
    exit 1
fi
ENCODED_IP=$(python3 ./utils/generate_encoded_ip.py "$1")
echo "Encoded server IP: $ENCODED_IP"

"$CONTAINER_RUNTIME" build --file Dockerfile.server --no-cache --tag "$SERVER_IMAGE" .

# Extract the binary from the built container so the same version can be given to the participants
"$CONTAINER_RUNTIME" build --file Dockerfile.build --build-arg SERVER_IP="$ENCODED_IP" --no-cache --tag "$BUILD_IMAGE" .
tmp_container_id=$($CONTAINER_RUNTIME create "$BUILD_IMAGE")
"$CONTAINER_RUNTIME" cp "$tmp_container_id":/build/boltmat .
"$CONTAINER_RUNTIME" rm "$tmp_container_id"

if [ -f "$CHALLENGE_NAME".zip ]; then
    rm "$CHALLENGE_NAME".zip
fi

zip -r $CHALLENGE_NAME.zip boltmat
