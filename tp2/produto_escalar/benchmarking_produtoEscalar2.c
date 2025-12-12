#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h> // Para MessageBox e medição de tempo de alta precisão
#include <time.h>

// Estrutura para passar dados para as threads
typedef struct {
    int id;
    int inicio;
    int fim;
    double* vetorA;
    double* vetorB;
    double somaParcial;
} ThreadData;

// --- VARIÁVEIS GLOBAIS ---
double* vetorA;
double* vetorB;
int TAMANHO_VETOR;
int QTD_THREADS;

// --- FUNÇÕES AUXILIARES DE TEMPO (Windows) ---
double get_time_ms() {
    LARGE_INTEGER frequency, start;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    return (double)start.QuadPart * 1000.0 / frequency.QuadPart;
}

// --- 1. ALGORITMO SEQUENCIAL ---
double produtoEscalarSequencial(int tamanho) {
    double soma = 0.0;
    for (int i = 0; i < tamanho; i++) {
        soma += vetorA[i] * vetorB[i];
    }
    return soma;
}

// --- 2. ALGORITMO PARALELO (Função da Thread) ---
void* threadWorker(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->somaParcial = 0.0;

    for (int i = data->inicio; i < data->fim; i++) {
        data->somaParcial += data->vetorA[i] * data->vetorB[i];
    }
    pthread_exit(NULL);
    return NULL;
}

// Função principal do modo paralelo
double produtoEscalarParalelo(int tamanho, int numThreads) {
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];
    double somaTotal = 0.0;
    int tamanhoBloco = tamanho / numThreads;

    // Criar threads
    for (int i = 0; i < numThreads; i++) {
        threadData[i].id = i;
        threadData[i].vetorA = vetorA;
        threadData[i].vetorB = vetorB;
        threadData[i].inicio = i * tamanhoBloco;
        
        // O último pega o resto (se a divisão não for exata)
        if (i == numThreads - 1) {
            threadData[i].fim = tamanho;
        } else {
            threadData[i].fim = (i + 1) * tamanhoBloco;
        }

        pthread_create(&threads[i], NULL, threadWorker, (void*)&threadData[i]);
    }

    // Esperar threads (Join) e somar parciais
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        somaTotal += threadData[i].somaParcial;
    }

    return somaTotal;
}

// --- SETUP E MAIN ---
void preencherVetores(int tamanho) {
    // Alocação dinâmica de memória (malloc) é necessária para vetores grandes em C
    vetorA = (double*)malloc(tamanho * sizeof(double));
    vetorB = (double*)malloc(tamanho * sizeof(double));

    // Preenchendo com valor fixo para teste (pode mudar para rand() se quiser)
    for (int i = 0; i < tamanho; i++) {
        vetorA[i] = 1.5;
        vetorB[i] = 2.0;
    }
}

int main() {
    // Para atender ao requisito de "Interativo" e "Janelas/Popups"
    MessageBox(NULL, "Bem-vindo ao Benchmark de Threads em C!\nClique OK para iniciar.", "Início", MB_OK | MB_ICONINFORMATION);

    printf("=== BENCHMARK DE PRODUTO ESCALAR (C/Pthreads) ===\n\n");
    
    // Entrada do usuário
    printf("Digite o tamanho do vetor (ex: 10000000): ");
    scanf("%d", &TAMANHO_VETOR);
    
    printf("Digite o numero de threads: ");
    scanf("%d", &QTD_THREADS);

    printf("\nAlocando memoria e gerando vetores...\n");
    preencherVetores(TAMANHO_VETOR);

    // --- TESTE SEQUENCIAL ---
    printf("Executando Sequencial...\n");
    double inicioSeq = get_time_ms();
    double resSeq = produtoEscalarSequencial(TAMANHO_VETOR);
    double fimSeq = get_time_ms();
    double tempoSeq = fimSeq - inicioSeq;
    printf(" Resultado Seq: %.2f | Tempo: %.4f ms\n", resSeq, tempoSeq);

    // --- TESTE PARALELO ---
    printf("Executando Paralelo (%d threads)...\n", QTD_THREADS);
    double inicioPar = get_time_ms();
    double resPar = produtoEscalarParalelo(TAMANHO_VETOR, QTD_THREADS);
    double fimPar = get_time_ms();
    double tempoPar = fimPar - inicioPar;
    printf(" Resultado Par: %.2f | Tempo: %.4f ms\n", resPar, tempoPar);

    // --- RESULTADOS FINAIS ---
    double speedup = tempoSeq / tempoPar;
    
    // Mostrando no Console
    printf("\n=== RELATORIO FINAL ===\n");
    printf("Speedup (Aceleracao): %.4fx\n", speedup);
    
    if (speedup > 1.0) {
        printf("Conclusao: O paralelo foi mais rapido!\n");
    } else {
        printf("Conclusao: O sequencial foi mais rapido (Overhead das threads foi maior).\n");
    }

    // --- POPUP FINAL (Requisito do professor) ---
    char msg[200];
    sprintf(msg, "Resultados:\nTempo Seq: %.2f ms\nTempo Par: %.2f ms\n\nSpeedup: %.2fx\n\n(Veja o console para detalhes)", 
            tempoSeq, tempoPar, speedup);
    
    MessageBox(NULL, msg, "Resultado do Benchmark", MB_OK | MB_ICONASTERISK);

    // Limpar memória
    free(vetorA);
    free(vetorB);

    return 0;
}