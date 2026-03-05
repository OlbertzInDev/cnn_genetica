#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define SEQ_LENGTH 4000
#define ALPHABET_SIZE 4 // A, C, G, T

// ============================================================================
// PHASE 1: DATASET PREP
// ============================================================================

// Gera a sequencia aleatoria simulando a cadeia genetica
void generate_sequence(char *sequence) {
    char nucleotides[] = {'A', 'C', 'G', 'T'};
    for (int i = 0; i < SEQ_LENGTH; i++) {
        sequence[i] = nucleotides[rand() % ALPHABET_SIZE];
    }
    sequence[SEQ_LENGTH] = '\0';
}

// Injeta um motivo/proteina na cadeia (Label 1) ou deixa aleatorio (Label 0)
void generate_training_sample(char *sequence, int *label) {
    generate_sequence(sequence);
    *label = rand() % 2;
    
    if (*label == 1) {
        char *target_motif = "GATTACA";
        int motif_len = strlen(target_motif);
        int insert_pos = rand() % (SEQ_LENGTH - motif_len);
        
        for(int i = 0; i < motif_len; i++) {
            sequence[insert_pos + i] = target_motif[i];
        }
    }
}

// Aplica o One-Hot Encoding (A=[1,0,0,0], etc)
void apply_one_hot(char *sequence, int one_hot_matrix[SEQ_LENGTH][ALPHABET_SIZE]) {
    for (int i = 0; i < SEQ_LENGTH; i++) {
        for (int j = 0; j < ALPHABET_SIZE; j++) one_hot_matrix[i][j] = 0;
        
        switch (sequence[i]) {
            case 'A': one_hot_matrix[i][0] = 1; break;
            case 'C': one_hot_matrix[i][1] = 1; break;
            case 'G': one_hot_matrix[i][2] = 1; break;
            case 'T': one_hot_matrix[i][3] = 1; break;
        }
    }
}

// ============================================================================
// PHASE 1.5: FILTER BANK GENERATION
// ============================================================================

// Backtracking para gerar a linguagem/todas combinacoes do alfabeto para tamanho K
void generate_combinations_recursive(char nucleotides[], char *current_filter, int filter_size, int depth, char **filter_list, int *list_index) {
    if (depth == filter_size) {
        current_filter[filter_size] = '\0';
        strcpy(filter_list[*list_index], current_filter);
        (*list_index)++;
        return;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        current_filter[depth] = nucleotides[i];
        generate_combinations_recursive(nucleotides, current_filter, filter_size, depth + 1, filter_list, list_index);
    }
}

// Aloca memoria e dispara a geracao do banco de filtros
char** create_filter_bank(int filter_size, int *total_filters) {
    *total_filters = 1;
    for (int i = 0; i < filter_size; i++) *total_filters *= ALPHABET_SIZE; 

    char **filter_list = (char **)malloc((*total_filters) * sizeof(char *));
    for (int i = 0; i < *total_filters; i++) {
        filter_list[i] = (char *)malloc((filter_size + 1) * sizeof(char));
    }

    char nucleotides[] = {'A', 'C', 'G', 'T'};
    char *current_filter = (char *)malloc((filter_size + 1) * sizeof(char));
    int index = 0;
    
    generate_combinations_recursive(nucleotides, current_filter, filter_size, 0, filter_list, &index);

    free(current_filter);
    return filter_list;
}

void free_filter_bank(char **filter_list, int total_filters) {
    for (int i = 0; i < total_filters; i++) free(filter_list[i]);
    free(filter_list);
}

// ============================================================================
// PHASE 2 & 8: CONVOLUTION & FLATTENING
// ============================================================================

// Converte um filtro string para matriz One-Hot
void apply_filter_one_hot(char *filter, int filter_size, int filter_matrix[][ALPHABET_SIZE]) {
    for (int i = 0; i < filter_size; i++) {
        for (int j = 0; j < ALPHABET_SIZE; j++) filter_matrix[i][j] = 0;
        switch (filter[i]) {
            case 'A': filter_matrix[i][0] = 1; break;
            case 'C': filter_matrix[i][1] = 1; break;
            case 'G': filter_matrix[i][2] = 1; break;
            case 'T': filter_matrix[i][3] = 1; break;
        }
    }
}

// Motor de convolucao 1D (Dot product da janela deslizante)
void conv1d(int seq_matrix[][ALPHABET_SIZE], char *filter, int filter_size, int *feature_map) {
    int filter_matrix[filter_size][ALPHABET_SIZE];
    apply_filter_one_hot(filter, filter_size, filter_matrix);

    int output_size = SEQ_LENGTH - filter_size + 1;
    
    for (int i = 0; i < output_size; i++) {
        int score = 0;
        for (int j = 0; j < filter_size; j++) {
            for (int c = 0; c < ALPHABET_SIZE; c++) {
                score += seq_matrix[i + j][c] * filter_matrix[j][c];
            }
        }
        feature_map[i] = score;
    }
}

// Roda a convolucao para o banco de filtros inteiro e stacka no flattened_vector
void multi_conv_flatten(int seq_matrix[][ALPHABET_SIZE], char **filter_bank, int num_filters, int filter_size, int *flattened_vector, int start_offset) {
    int feature_map_size = SEQ_LENGTH - filter_size + 1;
    int *temp_feature_map = (int*) malloc(feature_map_size * sizeof(int));
    
    for (int f = 0; f < num_filters; f++) {
        conv1d(seq_matrix, filter_bank[f], filter_size, temp_feature_map);
        
        int dest_pos = start_offset + (f * feature_map_size);
        for (int i = 0; i < feature_map_size; i++) {
            flattened_vector[dest_pos + i] = temp_feature_map[i];
        }
    }
    free(temp_feature_map);
}

// ============================================================================
// PHASE 6 & 7: MLP NEURAL NETWORK
// ============================================================================

double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_derivative(double pred) {
    return pred * (1.0 - pred); // A magica do calculo da sigmoide
}

// Inicializacao Xavier (Garante que os pesos nao saturem os neuronios no start)
double* initialize_weights(int size) {
    double *weights = (double*)malloc(size * sizeof(double));
    double limit = 1.0 / sqrt((double)size); 
    
    for (int i = 0; i < size; i++) {
        weights[i] = ((double)rand() / RAND_MAX) * (2.0 * limit) - limit;
    }
    return weights;
}

// Forward Pass: Soma ponderada + ativacao
double forward_pass_mlp(int *flattened_input, double *weights, double bias, int input_size) {
    double sum = bias;
    for (int i = 0; i < input_size; i++) {
        sum += flattened_input[i] * weights[i];
    }
    return sigmoid(sum);
}

// Backpropagation: Desce o gradiente e ajusta os pesos baseado no loss
void backward_pass_mlp(int *flattened_input, double pred, int true_label, double *weights, double *bias, int input_size, double lr) {
    double loss = pred - (double)true_label;
    double output_gradient = loss * sigmoid_derivative(pred);
    
    for (int i = 0; i < input_size; i++) {
        double weight_gradient = output_gradient * (double)flattened_input[i];
        weights[i] -= (lr * weight_gradient);
    }
    *bias -= (lr * output_gradient);
}

// ============================================================================
// MAIN EXECUTION
// ============================================================================

int main() {
    char sequence[SEQ_LENGTH + 1];
    int seq_matrix[SEQ_LENGTH][ALPHABET_SIZE];
    
    srand((unsigned int)time(NULL));

    printf("=== GENERATING FILTER BANKS ===\n");
    int total_k3, total_k5, total_k7;
    char **filters_k3 = create_filter_bank(3, &total_k3);
    char **filters_k5 = create_filter_bank(5, &total_k5);
    char **filters_k7 = create_filter_bank(7, &total_k7);
    
    printf("K=3 Filters: %d\nK=5 Filters: %d\nK=7 Filters: %d\n", total_k3, total_k5, total_k7);
    
    int out_k3 = total_k3 * (SEQ_LENGTH - 3 + 1);
    int out_k5 = total_k5 * (SEQ_LENGTH - 5 + 1);
    int out_k7 = total_k7 * (SEQ_LENGTH - 7 + 1);
    
    int mlp_input_size = out_k3 + out_k5 + out_k7;
    printf("\nTotal MLP input size (Flattened): %d ints (~%d MB)\n", 
           mlp_input_size, (mlp_input_size * 4) / (1024 * 1024));

    double *mlp_weights = initialize_weights(mlp_input_size);
    double mlp_bias = 0.0; 
    
    // Hyperparameters
    int epochs = 100; 
    double learning_rate = 0.0000001; 
    
    printf("\n=== TRAINING CNN (MULTI-SCALE FILTERS) ===\n");
    for (int step = 1; step <= epochs; step++) {
        int true_label;
        
        generate_training_sample(sequence, &true_label);
        apply_one_hot(sequence, seq_matrix);
        
        // Aloca os ~279MB do vetor achatado na heap
        int *flattened_vector = (int*) malloc(mlp_input_size * sizeof(int));
        if (!flattened_vector) {
            printf("CRITICAL ERROR: Out of Memory!\n"); return 1;
        }
        
        // Stacka as features no vetor
        multi_conv_flatten(seq_matrix, filters_k3, total_k3, 3, flattened_vector, 0);
        multi_conv_flatten(seq_matrix, filters_k5, total_k5, 5, flattened_vector, out_k3);
        multi_conv_flatten(seq_matrix, filters_k7, total_k7, 7, flattened_vector, out_k3 + out_k5);
        
        // Treino
        double pred = forward_pass_mlp(flattened_vector, mlp_weights, mlp_bias, mlp_input_size);
        double abs_error = fabs(pred - (double)true_label);
        
        if (step == 1 || step % 10 == 0) {
            printf("Step %03d | True Label: %d | Pred: %.4f | Abs Error: %.4f\n", 
                   step, true_label, pred, abs_error);
        }
        
        backward_pass_mlp(flattened_vector, pred, true_label, mlp_weights, &mlp_bias, mlp_input_size, learning_rate);
        
        free(flattened_vector); 
    }
    
    printf("\nTraining complete!\n");

    free(mlp_weights);
    free_filter_bank(filters_k3, total_k3);
    free_filter_bank(filters_k5, total_k5);
    free_filter_bank(filters_k7, total_k7);

    return 0;
}