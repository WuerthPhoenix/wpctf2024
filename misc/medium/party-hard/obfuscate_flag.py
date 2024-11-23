import base64

# Read the plaintext flag from the original file
with open("/root/original_flag.txt", "rb") as f:  # Corrected path
    plaintext_flag = f.read()

# Recursively base64 encode the flag 10 times
encoded_flag = plaintext_flag
for _ in range(40):
    encoded_flag = base64.b64encode(encoded_flag)

# Write the base64 encoded flag to the new file
with open("/root/flag.txt", "wb") as f:
    f.write(encoded_flag)

# Optionally, remove the original flag file
