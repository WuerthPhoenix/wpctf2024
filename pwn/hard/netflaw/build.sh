#!/bin/bash

FLAG="WPCTF{4_fl4w_1n_n3tfl0w_1s_f0r_n00bz}"
CH_NAME="netflaw"

docker build --file Dockerfile.build --build-arg CH_NAME="$CH_NAME" -o "${CH_NAME}" .

docker build --file Dockerfile --build-arg FLAG="$FLAG" --tag wpctf2024-challenges:${CH_NAME} ./${CH_NAME}
docker build --file Dockerfile.devel --build-arg FLAG="$FLAG" --tag wpctf2024-challenges:${CH_NAME}-devel ./${CH_NAME}

zip -r -j ${CH_NAME}.zip ${CH_NAME} Dockerfile

cp -f netflaw/* pwninit/
chown $USER:$USER pwninit/netflaw
#cd pwninit; make clean; make
