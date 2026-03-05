# Rede Neural Convolucional 1D para Sequências Genéticas

Este repositório contém o código-fonte de um projeto de Iniciação Científica. O objetivo é construir uma Rede Neural Convolucional (CNN) 1D inteiramente do zero em linguagem C (sem o uso de bibliotecas externas de Machine Learning) para identificar motivos de proteínas em sequências de DNA.

## 🧬 Contexto Científico

Diferente de CNNs tradicionais baseadas em imagens, esta rede opera em sequências 1D sobre o alfabeto genético $\Sigma = \{A, C, G, T\}$. Para capturar tanto padrões curtos quanto motivos mais longos e complexos, a arquitetura executa passagens convolucionais paralelas usando bancos de filtros completos de tamanhos variados ($k = 3, 5, 7$).

### Destaques da Arquitetura
1. **Codificação *One-Hot***: Sequências de texto são mapeadas para matrizes numéricas, permitindo operações matemáticas.
2. **Bancos de Filtros Combinatórios**: O algoritmo gera dinamicamente todas as permutações possíveis para os filtros de tamanho $k=3$ (64 filtros), $k=5$ (1024 filtros) e $k=7$ (16.384 filtros).
3. **Motor de Convolução 1D**: Calcula o produto escalar através de janelas deslizantes ao longo da cadeia de 4.000 caracteres.
4. **Achatamento (*Flattening*)**: Empilha os mapas de características (*feature maps*) gerados em um único vetor 1D massivo (aproximadamente 70 milhões de *features*).
5. **Rede Densa (MLP)**: Classificador final construído com Inicialização Xavier (Glorot), função de ativação Sigmoide e treinamento via *Backpropagation* (Descida do Gradiente) para prever a presença de uma proteína alvo.

## �� Como Executar (Docker)

Para evitar problemas de compatibilidade de compiladores (GCC/MinGW) e garantir que a máquina hospedeira consiga lidar com os picos de alocação de memória RAM na *Heap* (~279 MB por amostra), o projeto foi conteinerizado.

**1. Construir a Imagem Docker:**
No terminal, dentro da pasta do projeto, execute:
```bash
docker build -t cnn_genetica .# Rede Neural Convolucional 1D para Sequências Genéticas

Este repositório contém o código-fonte de um projeto de Iniciação Científica. O objetivo é construir uma Rede Neural Convolucional (CNN) 1D inteiramente do zero em linguagem C (sem o uso de bibliotecas externas de Machine Learning) para identificar motivos de proteínas em sequências de DNA.

## 🧬 Contexto Científico

Diferente de CNNs tradicionais baseadas em imagens, esta rede opera em sequências 1D sobre o alfabeto genético $\Sigma = \{A, C, G, T\}$. Para capturar tanto padrões curtos quanto motivos mais longos e complexos, a arquitetura executa passagens convolucionais paralelas usando bancos de filtros completos de tamanhos variados ($k = 3, 5, 7$).

### Destaques da Arquitetura
1. **Codificação *One-Hot***: Sequências de texto são mapeadas para matrizes numéricas, permitindo operações matemáticas.
2. **Bancos de Filtros Combinatórios**: O algoritmo gera dinamicamente todas as permutações possíveis para os filtros de tamanho $k=3$ (64 filtros), $k=5$ (1024 filtros) e $k=7$ (16.384 filtros).
3. **Motor de Convolução 1D**: Calcula o produto escalar através de janelas deslizantes ao longo da cadeia de 4.000 caracteres.
4. **Achatamento (*Flattening*)**: Empilha os mapas de características (*feature maps*) gerados em um único vetor 1D massivo (aproximadamente 70 milhões de *features*).
5. **Rede Densa (MLP)**: Classificador final construído com Inicialização Xavier (Glorot), função de ativação Sigmoide e treinamento via *Backpropagation* (Descida do Gradiente) para prever a presença de uma proteína alvo.

## �� Como Executar (Docker)

Para evitar problemas de compatibilidade de compiladores (GCC/MinGW) e garantir que a máquina hospedeira consiga lidar com os picos de alocação de memória RAM na *Heap* (~279 MB por amostra), o projeto foi conteinerizado.

**1. Construir a Imagem Docker:**
No terminal, dentro da pasta do projeto, execute:
```bash
docker build -t cnn_genetica .# Rede Neural Convolucional 1D para Sequências Genéticas

Este repositório contém o código-fonte de um projeto de Iniciação Científica. O objetivo é construir uma Rede Neural Convolucional (CNN) 1D inteiramente do zero em linguagem C (sem o uso de bibliotecas externas de Machine Learning) para identificar motivos de proteínas em sequências de DNA.

## 🧬 Contexto Científico

Diferente de CNNs tradicionais baseadas em imagens, esta rede opera em sequências 1D sobre o alfabeto genético $\Sigma = \{A, C, G, T\}$. Para capturar tanto padrões curtos quanto motivos mais longos e complexos, a arquitetura executa passagens convolucionais paralelas usando bancos de filtros completos de tamanhos variados ($k = 3, 5, 7$).

### Destaques da Arquitetura
1. **Codificação *One-Hot***: Sequências de texto são mapeadas para matrizes numéricas, permitindo operações matemáticas.
2. **Bancos de Filtros Combinatórios**: O algoritmo gera dinamicamente todas as permutações possíveis para os filtros de tamanho $k=3$ (64 filtros), $k=5$ (1024 filtros) e $k=7$ (16.384 filtros).
3. **Motor de Convolução 1D**: Calcula o produto escalar através de janelas deslizantes ao longo da cadeia de 4.000 caracteres.
4. **Achatamento (*Flattening*)**: Empilha os mapas de características (*feature maps*) gerados em um único vetor 1D massivo (aproximadamente 70 milhões de *features*).
5. **Rede Densa (MLP)**: Classificador final construído com Inicialização Xavier (Glorot), função de ativação Sigmoide e treinamento via *Backpropagation* (Descida do Gradiente) para prever a presença de uma proteína alvo.

## �� Como Executar (Docker)

Para evitar problemas de compatibilidade de compiladores (GCC/MinGW) e garantir que a máquina hospedeira consiga lidar com os picos de alocação de memória RAM na *Heap* (~279 MB por amostra), o projeto foi conteinerizado.

**1. Construir a Imagem Docker:**
No terminal, dentro da pasta do projeto, execute:
```bash
docker build -t cnn_genetica .
