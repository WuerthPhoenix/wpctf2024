FROM python:3.11.10-bullseye


ARG FLAG="WPCTF{fake_flag}"

ENV FLAG=${FLAG}
RUN curl -sL https://deb.nodesource.com/setup_20.x | bash - 

RUN apt update
RUN apt install -y wget curl supervisor gnupg nodejs chromium

RUN wget -q -O - https://packagecloud.io/varnishcache/varnish60lts/gpgkey | apt-key add -
RUN echo "deb https://packagecloud.io/varnishcache/varnish60lts/debian/ buster main" >> /etc/apt/sources.list.d/varnishcache_varnish60lts.list
RUN apt update
RUN DEBIAN_FRONTEND="noninteractive" apt install -y varnish 
RUN rm -rf /var/lib/apt/lists/*


COPY backend/requirements.txt /tmp/requirements.txt
RUN python3.11 -m pip install -r /tmp/requirements.txt

COPY bot/package.json /bot/package.json
RUN cd /bot && npm install

COPY bot /bot
COPY supervisord.conf /etc/supervisord.conf
COPY default.vcl /etc/varnish/default.vcl
COPY backend /app

EXPOSE 1337

CMD ["/usr/bin/supervisord","-c","/etc/supervisord.conf" ]
