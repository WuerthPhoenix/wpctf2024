#!/bin/bash
FLAG=$(cat flag.txt | tr -d '\n')
LEN=${#FLAG}
echo -e "${FLAG}\x1b[${LEN}DThese aren't the droids you're looking for."


