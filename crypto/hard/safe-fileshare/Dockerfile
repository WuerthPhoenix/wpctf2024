FROM python:3.11-slim

ARG FLAG="wpctf{test-flag}"
ARG FLASK_SESSION_KEY="wpctf-local-session"

ENV FLAG=${FLAG}
ENV FLASK_SESSION_KEY=${FLASK_SESSION_KEY}

WORKDIR /app

COPY challenge/. .
RUN pip install -r requirements.txt

EXPOSE 5000

CMD ["python", "main.py"]
