#!/usr/bin/env python3
import sys

def encode_ip(ip: str) -> str:
    encoded_ip = ""
    curr_key = 0x42
    for c in ip:
        encoded_char = ord(c) ^ curr_key
        encoded_ip += encoded_char.to_bytes(1, byteorder="little").hex()
        curr_key = encoded_char
    return encoded_ip


if __name__ == "__main__":
    ip = sys.argv[1]
    print(f"{encode_ip(ip)}")
