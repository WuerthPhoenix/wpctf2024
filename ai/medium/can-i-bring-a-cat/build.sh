#!/usr/bin/env bash

FLAG=$(cat flag.txt)
docker build -t wpctf2024-challenges:can-i-bring-a-cat --build-arg FLAG="$FLAG" .

rm -rf can_i_bring_a_cat.zip
zip -j can_i_bring_a_cat.zip challenge/src/*
