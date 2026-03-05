FROM debian:bullseye-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Usando o seu nome de arquivo correto
COPY cnn.c .

# Compilando o cnn.c
RUN gcc cnn.c -o cnn_exec -lm

CMD ["./cnn_exec"]