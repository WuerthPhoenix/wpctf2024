import base64

def base64_decode_until_flag(file_path, flag="WPCTF{"):
    # Read the encoded string from the file
    with open(file_path, 'r') as file:
        encoded_string = file.read().strip()

    decode_count = 0
    decoded_string = encoded_string
    
    # Keep decoding until the flag is found
    while flag not in decoded_string:
        try:
            # Decode the base64 string
            decoded_string = base64.b64decode(decoded_string).decode('utf-8')
            decode_count += 1
        except Exception as e:
            print(f"An error occurred during decoding: {e}")
            break

    # Print the final decoded string and number of iterations
    print(f"Decoded String: {decoded_string}")
    print(f"Total base64 decode iterations: {decode_count}")

# Example usage
file_path = "./obfuscated_flag.txt"
base64_decode_until_flag(file_path)
