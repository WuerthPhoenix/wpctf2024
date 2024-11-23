FROM maven:3.8.5-openjdk-17-slim AS builder

# Build the java application
WORKDIR /app
COPY java-app/pom.xml .
RUN mvn dependency:go-offline
COPY java-app/src/ ./src/
RUN mvn clean package -DskipTests=true

# Build the cat_flag binary
WORKDIR /cat_flag
COPY cat_flag/cat_flag.c .
RUN apt-get update && apt-get install -y gcc
RUN gcc -o cat_flag cat_flag.c


FROM eclipse-temurin:17-jdk-noble AS prod

# Create a non-root user
RUN addgroup --system appgroup && adduser --system --ingroup appgroup appuser

# Create directories
RUN mkdir /app
RUN mkdir /app/upload
RUN mkdir /app/templates

# Copy files
COPY inventory.xml /app/upload/inventory.xml
COPY templates /app/templates
COPY --from=builder /app/target/*.jar /app/app.jar
COPY --from=builder /cat_flag/cat_flag /usr/local/bin/cat_flag

# Set the flag
ARG FLAG="WPCTF{test_flag}"
ENV FLAG=${FLAG}
RUN echo ${FLAG} > /root/flag.txt

# Set permissions
RUN chmod 400 /root/flag.txt
RUN chown -R appuser:appgroup /app
RUN chmod 4755 /usr/local/bin/cat_flag

# Add /usr/bin to PATH
ENV PATH="/usr/local/bin:${PATH}"

# Change user and start the application
USER appuser
ENV SERVER_PORT=6060
WORKDIR /app
EXPOSE 6060
ENTRYPOINT ["java","-jar","app.jar"]