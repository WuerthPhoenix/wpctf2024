# Use Ubuntu 20.04 as the base image
FROM ubuntu@sha256:e341aa0d58bb9480bd092a137658bd1a7a5e8ae4fca31769df0eb7d6c4b8266e

# Install Python3, Flask, sudo, and cryptography package for encryption
RUN apt-get update && \
    apt-get install -y \
        python3 \
        python3-flask \
        python3-pip \
        sudo


# Create the vulnerable user (appuser)
RUN useradd -m appuser

# Configure sudoers file with a dangerous security configuration for appuser
RUN echo "appuser ALL=(ALL) NOPASSWD: /usr/bin/find" >> /etc/sudoers

WORKDIR /app

# Copy the Flask app code to the container
COPY app .

# Copy the obfuscation script
COPY obfuscate_flag.py /root/obfuscate_flag.py

# Copy the original flag file and encrypt it (as root)
COPY flag.txt /root/original_flag.txt
RUN python3 /root/obfuscate_flag.py && rm /root/original_flag.txt /root/obfuscate_flag.py

# Expose the Flask app on port 5023
EXPOSE 5023

# Switch to the non-root user for running the app
USER appuser

# Start the Flask web application
CMD ["python3", "app.py"]
