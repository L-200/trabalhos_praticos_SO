#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void preenche_matriz (int** matriz, int ordem) {
    printf("Criando matriz, isso pode levar um tempo...\n");
    int i, j;

    for(int i = 0; i < ordem; i++) {

        for (j = 0; j < ordem; j++) {
            matriz[i][j] = rand() % 1000;
        }
    }

    printf("Matriz criada com sucesso\n");
}

void multiplica_matrizes(int **A, int **B, int **C, int ordem) {
    printf("Multiplicando as matrizes. Isso pode levar mais tempo ainda...\n");
    for (int i = 0; i < ordem; i++) {
        for (int j = 0; j < ordem; j++) {
            C[i][j] = 0;
            for (int k = 0; k < ordem; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void free_matriz(int** matriz, int ordem) {
    int i;

    for (int i = 0; i < ordem; i++) {
        free(matriz[i]);
    }

    free(matriz);
}

int main() {

    struct timespec start, end;

    srand(time(NULL));

    printf("Defina a ordem das matrizes:");
    int ordem;
    scanf("%d", &ordem);
    clock_gettime(CLOCK_MONOTONIC, &start);

    int** A = cria_matriz(ordem);
    preenche_matriz(A, ordem);
    int** B = cria_matriz(ordem);
    preenche_matriz(B, ordem);

    int** C = cria_matriz(ordem);

    multiplica_matrizes(A, B, C, ordem);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double tempo_total = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("TEMPO DE EXECUÇÃO DO PROGRAMA: %.4f\n", tempo_total);

    free_matriz(A, ordem);
    free_matriz(B, ordem);
    free_matriz(C, ordem);

    return 0;
}