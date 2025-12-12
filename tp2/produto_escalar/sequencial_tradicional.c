#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int id;
    int inicio;
    int fim;
    double* vetorA;
    double* vetorB;
    double somaParcial;
} ThreadData;



// ----- VARIAVEIS GLOBAIS -------
double* vetorA;
double* vetorB;

int TAMANHO_VETOR;
int QTD_THREADS;





// --- 1. ALGORITMO SEQUENCIAL ---
double produtoEscalarSequencial(int tamanho) {
    double soma  = 0.0;
    for (int i = 0; i< tamanho; i++){
        soma += vetorA[i] * vetorB[i];
    }
    return soma;
}


// PreencherVetores com valores fixos
void preencherVetores(int tamanho){
    vetorA = (double*) malloc(tamanho * sizeof(double));
    vetorB = (double*) malloc(tamanho * sizeof(double));

    for (int i = 0; i < tamanho; i++){
        vetorA[i] = 1.5;
        vetorB[i] = 2.0;
    }
}


int main() {
    
    struct timespec start, end;

    printf("=========================== PRODUTO ESCALAR SEQUENCIAL (S/Pthreads) ===========================\n\n");


     // Entrada do usuário
    printf("Digite o tamanho do vetor (ex: 10000000): ");
    scanf("%d", &TAMANHO_VETOR);



    printf("Alocando memoria e gerando vetores\n\n");
    preencherVetores(TAMANHO_VETOR);


    // --- TESTE SEQUENCIAL ---
    printf("Executando Sequencial, Com %d elementos para cada vetor\n", TAMANHO_VETOR);
    clock_gettime(CLOCK_MONOTONIC, &start);
    srand(time(NULL));
    double resPar = produtoEscalarSequencial(TAMANHO_VETOR);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double tempo_total = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Resultado do Produto Escalar: %.2f | Tempo: %.4f ms\n\n", resPar, tempo_total);


    printf("===========================  FIM DO PRODUTO ESCALAR SEQUENCIAL (S/Pthreads) ===========================\n\n");


    // Limpar memória
    free(vetorA);
    free(vetorB);

    return 0;
}
