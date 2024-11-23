#!/bin/bash

CHALLENGE_NAME="party-hard"

docker run -d -p 5023:5023 --name misc-medium-misconfig wpctf2024-challenges:$CHALLENGE_NAME