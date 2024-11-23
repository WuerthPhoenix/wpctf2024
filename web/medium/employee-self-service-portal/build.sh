#!/bin/bash

CHALLENGE_NAME="employee-self-service-portal"

docker build --file Dockerfile --no-cache  --tag wpctf2024-challenges:$CHALLENGE_NAME .