#!/usr/bin/env python3

with open("./flag.txt") as f:
    FLAG = f.read().strip()
assert FLAG

checksum = 0

for c in FLAG:
    checksum ^= ord(c)

print(checksum)
