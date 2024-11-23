#!/bin/bash

CHALLENGE_NAME="party-hard"

docker build --file Dockerfile --no-cache --tag wpctf2024-challenges:$CHALLENGE_NAME .