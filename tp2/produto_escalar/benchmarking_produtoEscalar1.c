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



// --- FUNCOES AUXILIARES DE TEMPO (windows) ---
double get_time_ms() {
    LARGE_INTEGER frequency, start;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    return (double) start.QuadPart * 1000.0 / frequency.QuadPart;
}

// Perguntas -> como adaptar uma forma de conseguir o tempo que nao dependa de bibliotecas da maquina, por exemplo essa depende de que o SO seja o windows
// Caso nao tiver outra forma -> entender oque e como funciona essa funcao como: oque segnifica frequency e start e oque Query... faz, e o pq desse calculo.


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
    for (int i=data->inicio; i < data->fim; i++){
        data->somaParcial = data->vetorA[i] * data->vetorB[i];
    }
    thread_exit(NULL);
    return NULL;

}
//Perguntas -> como funciona em termos de memória quanto se tem um ponteiro para funcao e ponteiro para variaveis tipo void e Structs?
//Como funciona esse thread_exit(NUll) oque ira acontecer como vai saber que é esse thread que terminou e nao outras?
// porque precisa do (ThreadData*) antes do args? porque a funcao recebe void* sendo que ela vai receber um (ThreadData*)?


