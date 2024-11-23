#!/bin/bash

CHALLENGE_NAME="the-hacker-strikes-back"

docker build --file Dockerfile --no-cache --tag wpctf2024-challenges:$CHALLENGE_NAME .
