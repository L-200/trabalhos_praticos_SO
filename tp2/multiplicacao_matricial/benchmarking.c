// comando para compilar: gcc benchmarking.c matriz_paralela.c matriz_tradicional.c -o benchmarking -pthread
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "benchmarking.h"
#include "matriz_tradicional.h"
#include "matriz_paralela.h"

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

void free_matriz(int** matriz, int ordem) {
    int i;

    for (int i = 0; i < ordem; i++) {
        free(matriz[i]);
    }

    free(matriz);
}

int main() {
    srand(time(NULL)); // Inicializa o random uma única vez
    int opcao = 0;

    do {
        printf("\n========================================\n");
        printf("    BENCHMARK MATRIZES (TP2 SO)    \n");
        printf("========================================\n");
        printf("1. Executar Multiplicacao SEQUENCIAL\n");
        printf("2. Executar Multiplicacao PARALELA\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                chama_tradicional();
                break;
            case 2:
                chama_paralela();
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}