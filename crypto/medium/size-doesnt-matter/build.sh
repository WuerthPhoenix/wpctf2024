#!/bin/bash

CHALLENGE_NAME=$(echo "$(pwd)/$0" | xargs dirname -- | xargs basename --)

# Create docker image
docker build . -t "wpctf2024-challenges:$CHALLENGE_NAME"

# Create zip
mkdir "$CHALLENGE_NAME"
cp -r challenge flavor Dockerfile "$CHALLENGE_NAME"
echo "WPCTF{This_is_a_local_dummy_flag}" > "$CHALLENGE_NAME/flag.txt"
zip -r "$CHALLENGE_NAME.zip" "$CHALLENGE_NAME"
rm -rf "$CHALLENGE_NAME"
