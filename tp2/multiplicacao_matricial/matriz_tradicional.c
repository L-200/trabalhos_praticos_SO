#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "benchmarking.h"
#include "matriz_tradicional.h"

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

float chama_tradicional() {

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

    salvar_metricas("metricas_sequencial.csv", ordem, 1, tempo_total);

    return tempo_total;
}