FROM rockylinux:8

EXPOSE 9000
EXPOSE 8080/udp
ARG FLAG="WPCTF{test_flag}"

RUN dnf install -y socat glibc-2.28-251.el8_10.5.x86_64 gcc-8.5.0-22.el8_10.x86_64

RUN mkdir /app
RUN echo "              $FLAG" > /app/flag
RUN ln -s /dev/null /app/netflow.csv
COPY . /app/

RUN useradd -ms /bin/bash ctf

WORKDIR /app
USER ctf
CMD ["socat", "-dd", "TCP4-LISTEN:9000,fork,reuseaddr", "EXEC:\"/app/netflaw\",pty,stderr,setuid=ctf,raw"]