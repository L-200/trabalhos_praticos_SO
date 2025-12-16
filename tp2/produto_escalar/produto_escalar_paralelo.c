#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
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
int tamanhos_vet[21] = {100000,   5000000,  10000000, 15000000, 20000000, 25000000, 30000000 , 35000000 ,40000000, 45000000, 50000000,55000000, 60000000, 65000000, 70000000, 75000000, 80000000, 85000000 , 90000000, 95000000, 100000000};
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
// ---  PARSER PARA VALIDAÇÃO DOS ARGUMENTOS ---

int parseArgs(int argc, char* argv[]){
    if(argc<3 || argc>11){
        printf("Uso incorreto! Quantidade de argumentos insuficientes.\n");
        return 0;
    }
    else{
        for(int i=0; i<argc -1; i++){
            qtd_threads[i] = atoi(argv[i+1]);
        }
        return (argc -1);
    }
}


int main(int argc, char* argv[]) {
    // --- Declarações de variaveis locais da main ---
    struct timespec start, end;
    int validParser;
    FILE *arquivo;


    // --- CONFIGURAÇÕES INICIAIS DOS ARTEFATOS GERADOS ---
    arquivo = fopen("./dados_resultados/dados_paralelo.csv", "w");
    if( arquivo == NULL){
        printf("Erro ao abrir o arquivo");
        return 0;
    }
    fprintf(arquivo, "qtd_threads, tamanho_vet, resultado_PI, tempo(ms)\n");



    printf("=========================== PRODUTO ESCALAR PARALELO (C/Pthreads) ===========================\n\n");


    // --- VALIDAÇÃO PARA ARGUMENTOS DE THREADS PASSADAS PELO USUÁRIO ---
    QTD_THREADS_DIF = parseArgs(argc, argv);

    if( !QTD_THREADS_DIF ){
        return 0;
    }

    printf("Quantidade de versões a serem testadas: %d\n", QTD_THREADS_DIF);
    printf("Versões de threads a serem testadas: [");
    for(int i=0; i< QTD_THREADS_DIF; i++){
        if(i==QTD_THREADS_DIF-1){
            printf("%d]\n\n", qtd_threads[i]);
        }
        else{
            printf("%d ", qtd_threads[i]);
        }
    }


     // --- Entrada do usuário (TAMANHOS DE VETORES) ---
    printf("Digite a quantidade de tamanhos diferentes de vetor para comparação(ex: 2 resultará testes para: [100, 200]): ");
    scanf("%d", &QTD_TAMANHO_VET);


    


    // --- CÁLCULO PARA DIFERENTES VERSÕES ---
    for( int thread = 0 ; thread < QTD_THREADS_DIF; thread++){
        QTD_THREADS = qtd_threads[thread];


        for( int tamanho_vet = 0 ; tamanho_vet < QTD_TAMANHO_VET; tamanho_vet++){


            // --- DADOS INICIAIS NECESSÁRIOS ---
            TAMANHO_VETOR = tamanhos_vet[tamanho_vet];
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TAM VET %d ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", TAMANHO_VETOR);
            printf("\nAlocando memoria e gerando vetores...\n");
            preencherVetores(TAMANHO_VETOR);
            
            // --- DADOS PARA CONTROLE DAS THREADS ---
            pthread_t threads[QTD_THREADS];
            ThreadData threadsData[QTD_THREADS];
            int somaTotal = 0.0;
            int tamanhoBloco = TAMANHO_VETOR / QTD_THREADS;

            // --- TESTE PARALELO ---
            printf("Executando Paralelo (%d threads)\n\n", QTD_THREADS);

            // --- CÁLCULO DO PRODUTO INTERNO ---
            printf("============================ LOG DAS THREADS ============================\n");
            clock_gettime(CLOCK_MONOTONIC, &start);
            srand(time(NULL));

            // --- Criar threads e atribuir dados ---
            for (int i = 0; i < QTD_THREADS; i++) {
                threadsData[i].id = i;
                threadsData[i].vetorA = vetorA;
                threadsData[i].vetorB = vetorB;
                threadsData[i].inicio = i * tamanhoBloco;

                if( i == QTD_THREADS - 1 ){
                    threadsData[i].fim = TAMANHO_VETOR;
                } else {
                    threadsData[i].fim = (i + 1) * tamanhoBloco;
                }

                pthread_create(&threads[i], NULL, threadWorker, (void*)&threadsData[i]);
            }


            // --- Esperar threads (Join) e somar parciais somadas por elas ---
            for (int i = 0; i < QTD_THREADS; i++) {
                pthread_join(threads[i], NULL);
                somaTotal += threadsData[i].somaParcial;
            }
    
            // --- RESULTADO ----
            int resPar = somaTotal;
            clock_gettime(CLOCK_MONOTONIC, &end);
            printf("============================ FIM DAS THREADS ============================\n");


            // --- Analise e registro dos resultados ---
            double tempo_total = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("Resultado Par: %d | Tempo: %.4f ms\n\n", resPar, tempo_total);
            fprintf(arquivo, "%d, %d, %d, %.4f\n", QTD_THREADS, TAMANHO_VETOR, resPar, tempo_total);

            free(vetorA);
            free(vetorB);

        }
    }

    printf("===========================  FIM DO PRODUTO ESCALAR PARALELO (C/Pthreads) ===========================\n\n");

    // Limpar memória

    return 0;
}
