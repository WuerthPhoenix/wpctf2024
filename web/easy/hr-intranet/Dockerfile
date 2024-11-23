FROM node:20.18-alpine3.20

ARG FLAG="WPCTF{test_flag}"
ENV FLAG=$FLAG

# Create app directory
RUN mkdir /app

COPY src/package.json /app
COPY src/package-lock.json /app

WORKDIR /app

RUN npm install

COPY src/ /app

EXPOSE 3000

CMD ["npm", "start"]