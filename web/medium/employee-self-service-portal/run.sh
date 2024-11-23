#!/bin/bash

CHALLENGE_NAME="employee-self-service-portal"

docker run -d -p 1042:1042 --name $CHALLENGE_NAME wpctf2024-challenges:$CHALLENGE_NAME