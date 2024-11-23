# Use Python 3.9-slim as base image
FROM python@sha256:49f94609e5a997dc16086a66ac9664591854031d48e375945a9dbf4d1d53abbc

# Install necessary tools
RUN apt-get update && apt-get install -y \
    supervisor \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Create a new user and group
RUN groupadd -r webmediumctf && useradd -r -g webmediumctf webmediumctf

# Set up directories
WORKDIR /app

# Copy your application files
COPY . /app

# Change ownership of the app directory to the new user
RUN chown -R webmediumctf:webmediumctf /app

# Install any required Python packages (Flask, requests)
RUN pip install --no-cache-dir -r requirements.txt

# Copy the supervisord config file to manage multiple services
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

# Expose port 1042 for the public-facing service
EXPOSE 1042

# Switch to the new user
USER webmediumctf

# Start supervisord to manage services
CMD ["/usr/bin/supervisord"]
