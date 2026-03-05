#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define TAMANHO_CADEIA 4000
#define NUM_BASES 4 // Alfabeto genético: A, C, G, T

// ============================================================================
// FASE 1: PREPARAÇÃO DOS DADOS (DATASET)
// ============================================================================

/*
 * Função: gerar_sequencia
 * Objetivo: Simular uma cadeia genética de entrada.
 * Como funciona: Preenche um array de caracteres (string) sorteando letras 
 * do nosso alfabeto genético até atingir o TAMANHO_CADEIA.
 */
void gerar_sequencia(char *sequencia) {
    char nucleotideos[] = {'A', 'C', 'G', 'T'};
    for (int i = 0; i < TAMANHO_CADEIA; i++) {
        int index = rand() % 4; // Sorteia um número de 0 a 3
        sequencia[i] = nucleotideos[index];
    }
    sequencia[TAMANHO_CADEIA] = '\0'; // Finaliza a string padrão C
}

/*
 * Função: aplicar_one_hot
 * Objetivo: Traduzir texto para matemática. Redes Neurais não entendem letras.
 * Como funciona: Transforma cada letra em um vetor binário de 4 posições.
 * Exemplo: 'A' vira [1, 0, 0, 0], 'C' vira [0, 1, 0, 0].
 * O resultado é uma matriz bidimensional (TAMANHO_CADEIA x 4).
 */
void aplicar_one_hot(char *sequencia, int matriz_one_hot[TAMANHO_CADEIA][NUM_BASES]) {
    for (int i = 0; i < TAMANHO_CADEIA; i++) {
        // Primeiro, zera a linha inteira
        for (int j = 0; j < NUM_BASES; j++) matriz_one_hot[i][j] = 0;
        
        // Depois, "acende" (coloca 1) apenas na posição correspondente à letra
        switch (sequencia[i]) {
            case 'A': matriz_one_hot[i][0] = 1; break;
            case 'C': matriz_one_hot[i][1] = 1; break;
            case 'G': matriz_one_hot[i][2] = 1; break;
            case 'T': matriz_one_hot[i][3] = 1; break;
        }
    }
}

// ============================================================================
// FASE 1.5: CRIAÇÃO DOS FILTROS CONVOLUCIONAIS (KERNELS)
// ============================================================================

/*
 * Função: gerar_combinacoes_recursivo (Função Auxiliar / Backtracking)
 * Objetivo: Gerar todas as permutações possíveis para um tamanho de filtro K.
 * Como funciona: Explora uma "árvore" de possibilidades. Se K=3, ela escolhe 'A',
 * depois chama a si mesma para escolher a 2ª letra, e assim por diante. Quando
 * atinge K letras, salva a palavra gerada no nosso banco de filtros.
 */
void gerar_combinacoes_recursivo(char nucleotideos[], char *filtro_atual, int tamanho_filtro, int profundidade, char **lista_filtros, int *indice_lista) {
    // Caso Base: O filtro atingiu o tamanho desejado. Salvamos e voltamos.
    if (profundidade == tamanho_filtro) {
        filtro_atual[tamanho_filtro] = '\0';
        strcpy(lista_filtros[*indice_lista], filtro_atual);
        (*indice_lista)++;
        return;
    }

    // Exploração: Testa A, depois C, G, T na posição atual e avança a profundidade
    for (int i = 0; i < NUM_BASES; i++) {
        filtro_atual[profundidade] = nucleotideos[i];
        gerar_combinacoes_recursivo(nucleotideos, filtro_atual, tamanho_filtro, profundidade + 1, lista_filtros, indice_lista);
    }
}

/*
 * Função: criar_banco_filtros
 * Objetivo: Preparar a memória e disparar a geração de todos os filtros.
 * Como funciona: Calcula quantas combinações existirão (4^tamanho_filtro), 
 * aloca a memória dinâmica exata para guardar isso, e chama a função recursiva.
 */
char** criar_banco_filtros(int tamanho_filtro, int *total_filtros) {
    // Calcula 4 elevado à potência do tamanho_filtro (ex: 4^3 = 64)
    *total_filtros = 1;
    for (int i = 0; i < tamanho_filtro; i++) {
        *total_filtros *= NUM_BASES; 
    }

    // Aloca memória para a lista (Array de strings)
    char **lista_filtros = (char **)malloc((*total_filtros) * sizeof(char *));
    for (int i = 0; i < *total_filtros; i++) {
        lista_filtros[i] = (char *)malloc((tamanho_filtro + 1) * sizeof(char));
    }

    // Dispara a geração combinatória
    char nucleotideos[] = {'A', 'C', 'G', 'T'};
    char *filtro_atual = (char *)malloc((tamanho_filtro + 1) * sizeof(char));
    int indice = 0;
    gerar_combinacoes_recursivo(nucleotideos, filtro_atual, tamanho_filtro, 0, lista_filtros, &indice);

    free(filtro_atual); // Limpeza de variável temporária
    return lista_filtros;
}

// Função de segurança: Devolve a memória RAM ao sistema Operacional
void liberar_banco_filtros(char **lista_filtros, int total_filtros) {
    for (int i = 0; i < total_filtros; i++) free(lista_filtros[i]);
    free(lista_filtros);
}

// ============================================================================
// FASE 2: O MOTOR DE CONVOLUÇÃO (FORWARD PASS CONVOLUCIONAL)
// ============================================================================

/*
 * Função: aplicar_one_hot_filtro
 * Objetivo: Igual ao One-Hot da cadeia, mas focado em um filtro pequeno.
 * A convolução só acontece se o filtro e a cadeia falarem a mesma língua (números).
 */
void aplicar_one_hot_filtro(char *filtro, int tamanho_filtro, int matriz_filtro[][NUM_BASES]) {
    for (int i = 0; i < tamanho_filtro; i++) {
        for (int j = 0; j < NUM_BASES; j++) matriz_filtro[i][j] = 0;
        switch (filtro[i]) {
            case 'A': matriz_filtro[i][0] = 1; break;
            case 'C': matriz_filtro[i][1] = 1; break;
            case 'G': matriz_filtro[i][2] = 1; break;
            case 'T': matriz_filtro[i][3] = 1; break;
        }
    }
}

/*
 * Função: convolucao_1d
 * Objetivo: Extrair "Features" (Características) da cadeia.
 * Como funciona: Desliza o filtro passo a passo por toda a cadeia.
 * Em cada passo, multiplica os valores do filtro com o pedaço da cadeia (Produto Escalar).
 * O resultado é um mapa numérico indicando onde o filtro deu "match".
 */
void convolucao_1d(int matriz_cadeia[][NUM_BASES], char *filtro, int tamanho_filtro, int *feature_map) {
    int matriz_filtro[tamanho_filtro][NUM_BASES];
    aplicar_one_hot_filtro(filtro, tamanho_filtro, matriz_filtro);

    // Se a cadeia tem 4000 e o filtro 3, a janela desliza 3998 vezes.
    int tamanho_saida = TAMANHO_CADEIA - tamanho_filtro + 1;
    
    // Loop 1: Percorre a cadeia (Desliza a janela)
    for (int i = 0; i < tamanho_saida; i++) {
        int score = 0; // Guarda a pontuação desta janela
        
        // Loop 2 e 3: Produto escalar (Multiplica a janela atual pelo filtro)
        for (int j = 0; j < tamanho_filtro; j++) {
            for (int c = 0; c < NUM_BASES; c++) {
                score += matriz_cadeia[i + j][c] * matriz_filtro[j][c];
            }
        }
        feature_map[i] = score; // Salva o resultado no mapa de características
    }
}

// ============================================================================
// FASE 3: VISUALIZAÇÃO DOS DADOS (HEATMAP)
// ============================================================================

/*
 * Função: gerar_heatmap_ppm
 * Objetivo: Transformar os números do Feature Map em uma imagem visual.
 * Como funciona: Cria um arquivo de imagem texto puro (.ppm). 
 * Mapeia Scores altos para Verde e Scores baixos para Vermelho (Gradiente).
 */
void gerar_heatmap_ppm(const char *nome_arquivo, int *feature_map, int tamanho_mapa, int tamanho_filtro) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) return;

    int altura = 50; 
    int largura = tamanho_mapa;

    fprintf(arquivo, "P3\n%d %d\n255\n", largura, altura);

    for (int h = 0; h < altura; h++) {
        for (int w = 0; w < largura; w++) {
            int score = feature_map[w];
            float taxa = (float)score / tamanho_filtro; // Normaliza de 0.0 a 1.0
            
            // Lógica do Gradiente RGB
            int r = (int)((1.0 - taxa) * 255);
            int g = (int)(taxa * 255);
            int b = 0;

            fprintf(arquivo, "%d %d %d ", r, g, b);
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo);
}

/*
 * Função: convolucao_multipla_flatten (VERSÃO OTIMIZADA PARA MEMÓRIA)
 * Objetivo: Aplica um banco de filtros e escreve os resultados diretamente 
 * dentro de um "Super Vetor" pré-alocado, a partir de um offset específico.
 */
void convolucao_multipla_flatten(int matriz_cadeia[][NUM_BASES], char **banco_filtros, int num_filtros, int tamanho_filtro, int *super_vetor, int offset_inicial) {
    
    int tamanho_feature_map = TAMANHO_CADEIA - tamanho_filtro + 1;
    
    // Aloca apenas um pequeno vetor temporário para segurar 1 mapa por vez
    int *feature_map_temp = (int*) malloc(tamanho_feature_map * sizeof(int));
    
    for (int f = 0; f < num_filtros; f++) {
        // Convolui
        convolucao_1d(matriz_cadeia, banco_filtros[f], tamanho_filtro, feature_map_temp);
        
        // Calcula onde este mapa específico deve ser salvo dentro do Super Vetor
        int posicao_destino = offset_inicial + (f * tamanho_feature_map);
        
        // Copia os dados
        for (int i = 0; i < tamanho_feature_map; i++) {
            super_vetor[posicao_destino + i] = feature_map_temp[i];
        }
    }
    
    free(feature_map_temp);
}

void gerar_amostra_treino(char *sequencia, int *label) {
    // 1. Gera a sequência aleatória base (reaproveitando sua função)
    gerar_sequencia(sequencia);
    
    // 2. Sorteia cara ou coroa (0 ou 1)
    *label = rand() % 2;
    
    // 3. Se for 1, injetamos um padrão alvo no meio da cadeia para a CNN aprender
    if (*label == 1) {
        char *motivo_alvo = "GATTACA"; // O padrão que a rede terá que descobrir
        int tamanho_motivo = strlen(motivo_alvo);
        
        // Sorteia uma posição aleatória no meio da cadeia para esconder o motivo
        int pos_insercao = rand() % (TAMANHO_CADEIA - tamanho_motivo);
        
        // Sobrescreve a cadeia aleatória com o nosso padrão
        for(int i = 0; i < tamanho_motivo; i++) {
            sequencia[pos_insercao + i] = motivo_alvo[i];
        }
    }
}

void gerar_heatmap_2d(const char *nome_arquivo, int *vetor_achatado, int num_filtros, int tamanho_feature, int tamanho_filtro) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) return;

    // A imagem terá 3998 de largura e 64 de altura (1 pixel de altura para cada filtro)
    fprintf(arquivo, "P3\n%d %d\n255\n", tamanho_feature, num_filtros);

    for (int f = 0; f < num_filtros; f++) {
        for (int w = 0; w < tamanho_feature; w++) {
            // Pega o valor exato dentro do vetor achatado
            int score = vetor_achatado[f * tamanho_feature + w];
            float taxa = (float)score / tamanho_filtro; 

            int r = (int)((1.0 - taxa) * 255);
            int g = (int)(taxa * 255);
            
            fprintf(arquivo, "%d %d 0 ", r, g); // B é sempre 0
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo);
}

// ============================================================================
// FASE 6: MLP - ESTRUTURAS E FORWARD PASS
// ============================================================================

// A função de ativação que esmaga o resultado entre 0 e 1
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// ============================================================================
// FASE 7: BACKPROPAGATION E APRENDIZADO
// ============================================================================

/*
 * Função: derivada_sigmoid
 * O cálculo exige a derivada da função de ativação. A derivada matemática
 * da Sigmoide é incrivelmente elegante: f(x) * (1 - f(x)).
 * Como a nossa 'previsao' já é o resultado da sigmoide, usamos ela direto!
 */
double derivada_sigmoid(double previsao) {
    return previsao * (1.0 - previsao);
}

/*
 * Função: backward_pass_mlp
 * Objetivo: Ajustar os pesos para que a rede erre menos na próxima vez.
 */
void backward_pass_mlp(int *entrada_achatada, double previsao, int label_real, double *pesos, double *bias, int tamanho_entrada, double taxa_aprendizado) {
    
    // 1. Calcula a direção e o tamanho do erro (Loss)
    double erro = previsao - (double)label_real;
    
    // 2. Calcula o Gradiente da Saída (Regra da Cadeia do Cálculo)
    double gradiente_saida = erro * derivada_sigmoid(previsao);
    
    // 3. Atualiza cada um dos milhares de pesos
    for (int i = 0; i < tamanho_entrada; i++) {
        // A "culpa" do peso i depende se a entrada i estava ativa ou não
        double gradiente_peso = gradiente_saida * (double)entrada_achatada[i];
        
        // Atualiza o peso (Descida do Gradiente)
        pesos[i] = pesos[i] - (taxa_aprendizado * gradiente_peso);
    }
    
    // 4. Atualiza o viés (Bias), que não depende da entrada, apenas do gradiente
    *bias = *bias - (taxa_aprendizado * gradiente_saida);
}

// Aloca os Pesos dinamicamente e os inicializa com valores pequenos aleatórios
double* inicializar_pesos(int tamanho) {
    double *pesos = (double*)malloc(tamanho * sizeof(double));
    
    // O limite mágico: 1 dividido pela raiz quadrada do tamanho da entrada
    // Requer o #include <math.h> que já colocamos!
    double limite = 1.0 / sqrt((double)tamanho); 
    
    for (int i = 0; i < tamanho; i++) {
        // Sorteia um número decimal aleatório entre -limite e +limite
        pesos[i] = ((double)rand() / RAND_MAX) * (2.0 * limite) - limite;
    }
    return pesos;
}
// O Passe para Frente (O Neurônio "Pensando")
double forward_pass_mlp(int *entrada_achatada, double *pesos, double bias, int tamanho_entrada) {
    double soma = bias;
    
    // Multiplica cada feature pelo seu respectivo peso e acumula
    for (int i = 0; i < tamanho_entrada; i++) {
        soma += entrada_achatada[i] * pesos[i];
    }
    
    // Retorna a probabilidade (entre 0.0 e 1.0)
    return sigmoid(soma);
}


int main() {
    // --- O ALICERCE: CRIANDO E PREENCHENDO AS VARIÁVEIS ---
    char cadeia[TAMANHO_CADEIA + 1];
    int matriz_cadeia[TAMANHO_CADEIA][NUM_BASES];
    
    srand((unsigned int)time(NULL));
    
    // Gera a cadeia aleatória e já a converte para One-Hot Encoding
    gerar_sequencia(cadeia);
    aplicar_one_hot(cadeia, matriz_cadeia);
    // ------------------------------------------------------

    // === TESTE 3: GERACAO DOS FILTROS ===
    printf("=== GERANDO BANCOS DE FILTROS ===\n");
    int total_k3, total_k5, total_k7;
    
    char **filtros_k3 = criar_banco_filtros(3, &total_k3);
    char **filtros_k5 = criar_banco_filtros(5, &total_k5);
    char **filtros_k7 = criar_banco_filtros(7, &total_k7);
    
    printf("Filtros K=3: %d\n", total_k3);
    printf("Filtros K=5: %d\n", total_k5);
    printf("Filtros K=7: %d\n", total_k7);
    
    // Calcula o tamanho exato da Camada de Flattening
    int tam_out_k3 = total_k3 * (TAMANHO_CADEIA - 3 + 1);
    int tam_out_k5 = total_k5 * (TAMANHO_CADEIA - 5 + 1);
    int tam_out_k7 = total_k7 * (TAMANHO_CADEIA - 7 + 1);
    
    int tamanho_entrada_mlp = tam_out_k3 + tam_out_k5 + tam_out_k7;
    
    printf("\nTamanho total da entrada da MLP: %d inteiros (~%d MB)\n", 
           tamanho_entrada_mlp, (tamanho_entrada_mlp * 4) / (1024 * 1024));

    // Inicializa a MLP usando Inicialização Xavier para o novo tamanho gigante
    double *pesos_mlp = inicializar_pesos(tamanho_entrada_mlp);
    double bias_mlp = 0.0; 
    
    // Parâmetros de Treinamento
    int epocas = 100; // Reduzi para 100 para o teste rodar mais rápido
    double taxa_aprendizado = 0.0000001; // Reduzimos a taxa ainda mais, pois a entrada é muito maior agora!
    
    printf("\n=== TREINANDO A CNN COM FILTROS MULTIPLOS ===\n");
    for (int passo = 1; passo <= epocas; passo++) {
        int label_real;
        
        // 1. Gera e codifica a amostra
        gerar_amostra_treino(cadeia, &label_real);
        aplicar_one_hot(cadeia, matriz_cadeia);
        
        // 2. Aloca o Super Vetor (279 MB) na RAM
        int *super_vetor = (int*) malloc(tamanho_entrada_mlp * sizeof(int));
        if (!super_vetor) {
            printf("FALTA DE MEMORIA RAM!\n"); return 1;
        }
        
        // 3. Aplica as 3 escalas de convolução, empilhando os dados
        // K=3 começa no índice 0
        convolucao_multipla_flatten(matriz_cadeia, filtros_k3, total_k3, 3, super_vetor, 0);
        
        // K=5 começa logo após o K=3
        convolucao_multipla_flatten(matriz_cadeia, filtros_k5, total_k5, 5, super_vetor, tam_out_k3);
        
        // K=7 começa logo após o K=5
        convolucao_multipla_flatten(matriz_cadeia, filtros_k7, total_k7, 7, super_vetor, tam_out_k3 + tam_out_k5);
        
        // 4. Forward Pass e Backward Pass
        double previsao_rede = forward_pass_mlp(super_vetor, pesos_mlp, bias_mlp, tamanho_entrada_mlp);
        double erro_absoluto = fabs(previsao_rede - (double)label_real);
        
        if (passo == 1 || passo % 10 == 0) {
            printf("Passo %03d | Label: %d | Previsao: %.4f | Erro: %.4f\n", 
                   passo, label_real, previsao_rede, erro_absoluto);
        }
        
        backward_pass_mlp(super_vetor, previsao_rede, label_real, pesos_mlp, &bias_mlp, tamanho_entrada_mlp, taxa_aprendizado);
        
        // 5. FAXINA OBRIGATÓRIA (Libera os 279 MB desta iteração)
        free(super_vetor); 
    }
    
    printf("\nTreinamento concluído!\n");

    // Limpeza final geral
    free(pesos_mlp);
    liberar_banco_filtros(filtros_k3, total_k3);
    liberar_banco_filtros(filtros_k5, total_k5);
    liberar_banco_filtros(filtros_k7, total_k7);

    return 0;
}