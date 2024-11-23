#!/usr/bin/env bash

CHALLENGE_NAME=$(echo "$(pwd)/$0" | xargs dirname -- | xargs basename --)

# Create docker container
FLASK_SESSION_KEY="$(cat /dev/urandom | tr -c -d "0-9a-f" | head -c 32)"
FLAG="$(cat flag.txt)"
docker build . -t "wpctf2024-challenges:$CHALLENGE_NAME" --build-arg "FLAG=$FLAG" --build-arg "FLASK_SESSION_KEY=$FLASK_SESSION_KEY"

# Create zip file
mkdir "$CHALLENGE_NAME"
cp -r challenge Dockerfile "$CHALLENGE_NAME"

# Replace the secrets with easier secrets for local testing of the exploit.
mv "$CHALLENGE_NAME/challenge/safe_fileshare/secrets.py.local" "$CHALLENGE_NAME/challenge/safe_fileshare/secrets.py"
zip -r "$CHALLENGE_NAME.zip" "$CHALLENGE_NAME"
rm -rf "$CHALLENGE_NAME"