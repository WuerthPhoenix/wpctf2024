FROM python:3.11-slim

ARG FLAG
ENV FLAG=$FLAG

WORKDIR /app

COPY challenge/. .

RUN pip install -r src/requirements.txt

CMD ["fastapi", "run", "src/app.py"]
EXPOSE 8000
