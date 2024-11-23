FROM ubuntu:24.04 AS builder

RUN apt-get update \
    && apt-get -y --no-install-recommends install \
    build-essential \
    libssl-dev

WORKDIR /build
COPY Makefile ./
COPY ./src ./src

RUN make

FROM ubuntu:24.04

RUN apt-get update && apt-get -y --no-install-recommends install socat

WORKDIR /app
ARG FLAG=WPCTF{redacted}

COPY --from=builder /build/scanner ./scanner
COPY ./scanner-devices ./scanner-devices

RUN echo $FLAG > ./scanner-devices/ceo-office

RUN useradd -m limiteduser \
    && chown -R limiteduser:limiteduser ./scanner ./scanner-devices \
    && chmod 744 ./scanner

USER limiteduser

EXPOSE 45254

CMD ["socat", "TCP-LISTEN:45254,reuseaddr,fork", "EXEC:./scanner"]
