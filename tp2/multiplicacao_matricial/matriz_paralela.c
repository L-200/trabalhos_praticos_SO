#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef struct {
    int thread_id;
    int num_threads;
    int ordem;
    int** A;
    int** B; 
    int** C;
} ThreadData;

int** cria_matriz(int ordem) {

    int **matriz = (int**) malloc(ordem* sizeof(int*));
    if (matriz == NULL) {
        printf("ERRO DE ALOCAÇÃO DE LINHAS\n");
    } 

    for (int i = 0; i < ordem; i++) {
        matriz[i] = (int*) malloc (ordem * sizeof(int));
        if (matriz[i] ==NULL) {
            printf("ERRO DE ALOCAÇÃO DE COLUNAS\n");
        }
    }

    return matriz;
}

void preenche_matriz(int** matriz, int ordem) {
    printf("Preenchendo uma matriz, isso pode levar um tempinho...\n");
    int i, j;

    for (i = 0; i < ordem; i++) {

        for (j = 0; j < ordem; j++) {
            matriz[i][j] = rand() % 1000;
        }
    }
}

void free_matriz (int** matriz, int ordem) {
    int i;

    for (i = 0; i < ordem; i++) {
        free(matriz[i]);
    }

    free(matriz);
}

// MÉTODO ESCOLHIDO: dividir as linhas pela quantidade de threads disponiveis

void *thread_multiplica(void* args) {
    ThreadData *dados = (ThreadData* ) args;
    printf("Thread de multiplicação número %d iniciada.\n", dados->thread_id);

    //calculo da divisão de linhas
    int n = dados->ordem;
    int linhas_por_thread = n/dados->num_threads;

    int inicio = dados->thread_id * linhas_por_thread;
    int fim;

    if (dados->thread_id == dados->num_threads - 1) { // se a carga não poder ser dividida igualmente a última pega as linhas que faltaram
        fim = n;
    } else {
        fim = (dados->thread_id + 1) * linhas_por_thread;
    }

    // loop de multiplicação
    for (int i = inicio; i < fim; i++) {
        for (int j = 0; j < n; j++) {

            int soma = 0;
            for (int k = 0; k < n; k++) {
                soma += dados->A[i][k] * dados->B[k][j];
            }
            dados->C[i][j] = soma;
        }
    }

    printf("Thread de multiplicação %d encerrada com sucesso\n", dados->thread_id);
    pthread_exit(NULL);
}

int main () {

    struct timespec start, end;
    srand(time(NULL));

    int ordem, num_threads;
    printf("Defina a ordem das matrizes: ");
    scanf("%d", &ordem);
    printf("Defina a quantidade de threads multiplicadoras: ");
    scanf("%d", &num_threads);

    clock_gettime(CLOCK_MONOTONIC, &start);
    int** A = cria_matriz(ordem);
    preenche_matriz(A, ordem);
    int** B = cria_matriz(ordem);
    preenche_matriz(B, ordem);
    int** C = cria_matriz(ordem);   

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    for(int i = 0; i < num_threads; i++) {
        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].C = C;
        thread_data[i].num_threads = num_threads;
        thread_data[i].ordem = ordem;
        thread_data[i].thread_id = i;

        pthread_create(&threads[i], NULL, thread_multiplica, (void *)&thread_data[i]);
    }

    // Esperar todas as threads acabarem
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double tempo_total = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("TEMPO DE EXECUÇÃO: %.4f segundos\n", tempo_total);

    free_matriz(A, ordem);
    free_matriz(B, ordem);
    free_matriz(C, ordem);

    return 0;
}