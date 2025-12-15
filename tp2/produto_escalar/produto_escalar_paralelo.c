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
int tamanhos_vet[10] = {10000, 50000, 100000, 250000, 500000, 1000000, 2000000, 3000000, 5000000, 6000000};
int qtd_threads[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10};

int TAMANHO_VETOR;
int QTD_TAMANHO_VET;
int QTD_THREADS;
int QTD_THREADS_DIF;



// --- ALGORITMO PARALELO ---
void* threadWorker(void* args){
    // --- VARIAVEIS NECESÁRIAS ---
    ThreadData* data = (ThreadData*) args;
    data->somaParcial = 0.0;

    // --- SOMA PARCIAL ---
    printf("Thread %d: Calculando de %d a %d\n", data->id, data->inicio, data->fim);
    for (int i=data->inicio; i < data->fim; i++){
        data->somaParcial += data->vetorA[i] * data->vetorB[i];
    }

    // --- RESULTADOS ---
    printf("Thread %d: Soma Parcial = %.2f\n", data->id, data->somaParcial);
    pthread_exit(NULL);
    return NULL;

}

// --- Função principal do modo paralelo ---
double produtoEscalarParalelo(int tamanho, int numThreads){
    // --- VARIÁVEIS NECESSÁRIAS --- 
    pthread_t threads[numThreads];
    ThreadData threadsData[numThreads];
    double somaTotal = 0.0;
    int tamanhoBloco = tamanho / numThreads;

    // --- Criar threads e atribuir dados ---
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


    // --- Esperar threads (Join) e somar parciais somadas por elas ---
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        somaTotal += threadsData[i].somaParcial;
    }

    // --- RESULTADO FINAL ---
    return somaTotal;
}


// --- PreencherVetores com valores fixos ---
void preencherVetores(int tamanho){
    vetorA = (double*) malloc(tamanho * sizeof(double));
    vetorB = (double*) malloc(tamanho * sizeof(double));

    for (int i = 0; i < tamanho; i++){
        vetorA[i] = 1.5;
        vetorB[i] = 2.0;
    }
}


int main() {
    // --- Declarações de variaveis locais da main ---
    struct timespec start, end;
    FILE *arquivo;


    // --- CONFIGURAÇÕES INICIAIS DOS ARTEFATOS GERADOS ---
    arquivo = fopen("./dados_resultados/dados_paralelo.csv", "w");
    if( arquivo == NULL){
        printf("Erro ao abrir o arquivo");
        return 0;
    }
    fprintf(arquivo, "qtd_threads, tamanho_vet, resultado_PI, tempo(ms)\n");



    printf("=========================== PRODUTO ESCALAR PARALELO (C/Pthreads) ===========================\n\n");

     // --- Entrada do usuário ---
    printf("Digite a quantidade de tamanhos diferentes de vetor para comparação(ex: 2 resultará testes para: [100, 200]): ");
    scanf("%d", &QTD_TAMANHO_VET);


    printf("Digite o quntidade diferentes para o  numero de threads: ");
    scanf("%d", &QTD_THREADS_DIF);


    // --- CÁLCULO PARA DIFERENTES VERSÕES ---
    for( int thread = 0 ; thread < QTD_THREADS_DIF; thread++){
        QTD_THREADS = qtd_threads[thread];


        for( int tamanho_vet = 0 ; tamanho_vet < QTD_TAMANHO_VET; tamanho_vet++){

            // --- DADOS INICIAIS NECESSÁRIOS ---
            TAMANHO_VETOR = tamanhos_vet[tamanho_vet];
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TAM VET %d ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", TAMANHO_VETOR);
            printf("\nAlocando memoria e gerando vetores...\n");
            preencherVetores(TAMANHO_VETOR);


            // --- TESTE PARALELO ---
            printf("Executando Paralelo (%d threads)\n\n", QTD_THREADS);

            // --- CÁLCULO DO PRODUTO INTERNO ---
            printf("============================ LOG DAS THREADS ============================\n");
            clock_gettime(CLOCK_MONOTONIC, &start);
            srand(time(NULL));


            double resPar = produtoEscalarParalelo(TAMANHO_VETOR, QTD_THREADS);
            clock_gettime(CLOCK_MONOTONIC, &end);
            printf("============================ FIM DAS THREADS ============================\n");


            // --- Analise e registro dos resultados ---
            double tempo_total = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("Resultado Par: %.2f | Tempo: %.4f ms\n\n", resPar, tempo_total);
            fprintf(arquivo, "%d, %d, %.2f, %.4f\n", QTD_THREADS, TAMANHO_VETOR, resPar, tempo_total);

            free(vetorA);
            free(vetorB);

        }
    }

    printf("===========================  FIM DO PRODUTO ESCALAR PARALELO (C/Pthreads) ===========================\n\n");

    // Limpar memória

    return 0;
}
