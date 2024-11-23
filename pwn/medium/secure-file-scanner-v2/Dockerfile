FROM ubuntu:24.04 AS builder

RUN apt-get update \
    && apt-get -y --no-install-recommends install \
    build-essential \
    libssl-dev

WORKDIR /build
COPY Makefile ./

COPY ./src ./src

RUN make

# Use ubuntu because gcompat in alpine breaks the vulnerability
FROM ubuntu:24.04

RUN apt-get update && apt-get -y --no-install-recommends install socat

WORKDIR /app
ARG FLAG=WPCTF{redacted}

COPY --from=builder /build/scanner-v2 ./scanner-v2
COPY ./scanner-devices ./scanner-devices
COPY flag.txt ./flag.txt

RUN useradd -m limiteduser \
    && chown -R limiteduser:limiteduser ./scanner-v2 \
    && chmod 744 ./scanner-v2

USER limiteduser

EXPOSE 58946

CMD ["socat", "TCP-LISTEN:58946,reuseaddr,fork", "EXEC:./scanner-v2"]
