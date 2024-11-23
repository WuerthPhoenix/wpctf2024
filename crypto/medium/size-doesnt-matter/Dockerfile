FROM rust:1.79 AS builder

# Set workdir
WORKDIR /usr/src/crypto-medium

COPY challenge .

# Run rust tests and build the project
RUN cargo test --release
RUN cargo build --release

# Run the challenge on debian
FROM debian:stable-slim

# Create user and home directory
ARG DEFAULT_USER=crypto-medium
RUN groupadd $DEFAULT_USER
RUN useradd $DEFAULT_USER -g $DEFAULT_USER

# Copy executable
COPY --from=builder /usr/src/crypto-medium/target/release/crypto-medium /bin/crypto-medium

# Create certificates
RUN apt update && apt install openssl -y
RUN mkdir -p '/var/crypto-medium/conf/certs'
RUN openssl req -x509 \
        -newkey rsa:4096 \
        -keyout /var/crypto-medium/conf/certs/crypto-medium.key \
        -out    /var/crypto-medium/conf/certs/crypto-medium.crt \
        -sha256 \
        -days 3650 \
        -nodes \
        -subj "/C=IT/ST=Italy/L=Bolzano/O=WuerthPhoenix/OU=SI/CN=crypto-medium"

# Create verification-token
RUN cat /dev/urandom | tr -c -d "[:alnum:]" | head -c 32 > /var/crypto-medium/conf/token

# Set permissions
RUN chown -R $DEFAULT_USER:$DEFAULT_USER /var/crypto-medium/

# Copy flag
COPY flavor /var/crypto-medium/data/
COPY flag.txt /var/crypto-medium/data/flag.txt

# set user
USER $DEFAULT_USER

# set container work directory
WORKDIR /var/crypto-medium/data

# Expose the used port
EXPOSE 8000

# set entrypoint
ENTRYPOINT [ "/bin/crypto-medium" ]