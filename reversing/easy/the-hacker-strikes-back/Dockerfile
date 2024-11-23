# Usa una base leggera come Alpine
FROM alpine:3.20.3

# Installa le utility necessarie
RUN apk add --no-cache bash coreutils openrc socat

# Crea una cartella per la shell limitata
RUN mkdir /chroot
RUN mkdir /chroot/bin

COPY flag.txt /flag.txt
COPY obfuscate_flag.sh /obfuscate_flag.sh
RUN /bin/sh /obfuscate_flag.sh > /chroot/flag.txt

RUN cp /bin/ls /chroot/bin/ls
RUN cp /bin/cat /chroot/bin/cat
RUN cp /bin/sh /chroot/bin/sh

RUN mkdir -p /chroot/lib
RUN cp -r /lib/* /chroot/lib/

# Crea un utente con UID 1000
RUN adduser -D -u 1000 limiteduser

# Modifica il file passwd per l'utente limitato
RUN echo "limiteduser:x:1000:1000::/chroot:/bin/sh" >> /etc/passwd

# Espone la porta 8080
EXPOSE 8080

WORKDIR /chroot

CMD ["socat", "TCP-LISTEN:8080,reuseaddr,fork", "EXEC:/usr/sbin/chroot /chroot /bin/sh"]