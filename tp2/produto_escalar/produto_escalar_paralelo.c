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

void* threadWorker(void* args){
    ThreadData* data = (ThreadData*) args;
    data->somaParcial = 0.0;
    printf("Thread %d: Calculando de %d a %d\n", data->id, data->inicio, data->fim);
    for (int i=data->inicio; i < data->fim; i++){
        data->somaParcial += data->vetorA[i] * data->vetorB[i];
    }
    printf("Thread %d: Soma Parcial = %.2f\n", data->id, data->somaParcial);
    pthread_exit(NULL);
    return NULL;

}

// Função principal do modo paralelo
double produtoEscalarParalelo(int tamanho, int numThreads){
    pthread_t threads[numThreads];
    ThreadData threadsData[numThreads];
    double somaTotal = 0.0;
    int tamanhoBloco = tamanho / numThreads;

    // Criar threads e atribuir dados
    for (int i = 0; i < numThreads; i++) {
        threadsData[i].id = i;
        threadsData[i].vetorA = vetorA;
        threadsData[i].vetorB = vetorB;
        threadsData[i].inicio = i * tamanhoBloco;

        if( i == numThreads - 1 ){
            threadsData[i].fim = tamanho;
        } else {
            threadsData[i].fim = (i + 1) * tamanhoBloco;
        }

        pthread_create(&threads[i], NULL, threadWorker, (void*)&threadsData[i]);
    }


    // Esperar threads (Join) e somar parciais somadas por elas
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        somaTotal += threadsData[i].somaParcial;
    }
    return somaTotal;
}

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

    printf("=========================== PRODUTO ESCALAR PARALELO (C/Pthreads) ===========================\n\n");

     // Entrada do usuário
    printf("Digite o tamanho do vetor (ex: 10000000): ");
    scanf("%d", &TAMANHO_VETOR);


    printf("Digite o numero de threads: ");
    scanf("%d", &QTD_THREADS);

    printf("\nAlocando memoria e gerando vetores...\n");
    preencherVetores(TAMANHO_VETOR);


    // --- TESTE PARALELO ---
    printf("Executando Paralelo (%d threads)\n\n", QTD_THREADS);


    printf("============================ LOG DAS THREADS ============================\n");
    clock_gettime(CLOCK_MONOTONIC, &start);
    srand(time(NULL));
    double resPar = produtoEscalarParalelo(TAMANHO_VETOR, QTD_THREADS);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("============================ FIM DAS THREADS ============================\n");


    // --- Calculo do tempo ---
    double tempo_total = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Resultado Par: %.2f | Tempo: %.4f ms\n\n", resPar, tempo_total);



    printf("===========================  FIM DO PRODUTO ESCALAR PARALELO (C/Pthreads) ===========================\n\n");

    // Limpar memória
    free(vetorA);
    free(vetorB);

    return 0;
}
