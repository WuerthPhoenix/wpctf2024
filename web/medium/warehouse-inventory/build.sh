#!/bin/bash

CHALLENGE_NAME="warehouse-inventory"
FLAG=$(cat flag.txt)
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

cp -r ./java-app ./Dockerfile ./templates ./cat_flag ./inventory.xml "$WORK_DIR"
(cd $WORK_DIR && sed -i 's/Pattern.compile.*/REDACTED/g' java-app/src/main/java/com/acme/inventory/InventoryApplication.java && zip -r "$SCRIPT_DIR/$CHALLENGE_NAME.zip" .)

docker build --file Dockerfile --no-cache --tag wpctf2024-challenges:$CHALLENGE_NAME --build-arg FLAG="$FLAG" .