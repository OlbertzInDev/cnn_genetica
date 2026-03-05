# Usamos uma imagem base leve do Debian
FROM debian:bullseye-slim

# Atualizamos os pacotes e instalamos o compilador C
RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Criamos o diretório de trabalho dentro do contêiner
WORKDIR /app

# Copiamos o nosso código para dentro do contêiner
COPY cnn_genetica.c .

# Compilamos o código na hora que a imagem é construída (linkando o math.h com -lm)
RUN gcc cnn_genetica.c -o cnn_exec -lm

# Quando o contêiner rodar, ele executa o nosso programa compilado
CMD ["./cnn_exec"]
