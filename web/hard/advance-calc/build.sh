#!/bin/bash

CHALLENGE_NAME="advanced-calc"
SCRIPT_DIR=$(pwd)
WORK_DIR=`mktemp -d -p "$DIR"`
if [[ ! "$WORK_DIR" || ! -d "$WORK_DIR" ]]; then
  echo "Could not create temp dir"
  exit 1
fi

function cleanup {      
  rm -rf "$WORK_DIR"
  echo "Deleted temp working directory $WORK_DIR"
}

trap cleanup EXIT

# Get flag from file and trim
FLAG=$(cat flag.txt | tr -d '\n')

cp . $WORK_DIR -r
(cd $WORK_DIR && xargs rm -rf < .zipignore &&  sed -i 's/\(30319 \* 32299\)/REDACTED/g' src/backend/app.py &&zip -r "$SCRIPT_DIR/$CHALLENGE_NAME.zip" .)
docker build --file Dockerfile --no-cache --build-arg FLAG="$FLAG"  --tag wpctf2024-challenges:$CHALLENGE_NAME -f Dockerfile src/
