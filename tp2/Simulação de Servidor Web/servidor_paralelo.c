// comando para compilar: gcc servidor_paralelo.c simulacao_de_funcoes.c -o servidor_paralelo
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "simulacao_de_funcoes.h" // Inclui as funções de simulação

typedef struct {
    int id;
    int tipo_req;
} ThreadArgs;

void trata_requisicao(void* Args) {
    ThreadArgs *dados = (ThreadArgs*) Args;

    printf("\n[SERVIDOR]: Cliente %d Chegou. Processando...\n", dados->id);

    switch(dados->tipo_req) {
        case 1: 
        processar_login(dados->tipo_req); 
        break;
        
        case 2:
        processar_html(dados->tipo_req);
        break;
        
        case 3:
        processar_consulta(dados->tipo_req);
        break;
        
        case 4:
        processar_imagem(dados->tipo_req);
        break;
        
        case 5:
        processar_video(dados->tipo_req);
        break;
    }

    // liberando a memória alocada para os argumentos dessa thread
    free(dados);

    pthread_exit(NULL);
}

int main () {
    
    time_t inicio, fim;
    double tempo_total;

    int num_clientes;
    int i;

    printf("--- SERVIDOR WEB SIMULADO: VERSAO PARALELA ---\n");
    printf("OBJETIVO: Demonstracao do paralelismo com threads.\n");
    printf("\n[SERVIDOR]: Digite a quantidade de clientes (requisicoes) a simular: ");

    if (scanf("%d", &num_clientes) != 1 || num_clientes <= 0) {
        printf("[ERRO]: Entrada invalida. Encerrando.\n");
        return 1;
    }

    printf("\n[SERVIDOR]: Simulacao iniciada para %d clientes. Cada um sera atendido em paralelo...\n", num_clientes);
    printf("OBS: Uma thread pode acabar no momento em que você está digitando a próxima requisição!\n");
    printf("Apenas continue digitando a requisição normalmente caso isso ocorra.\n");
    printf("É recomendável que você digite as requisições rapidamente para ver o efeito do paralelismo.\n");

    inicio = time(NULL);

    pthread_t threads[num_clientes];
    for (i = 1; i <= num_clientes; i++) {
        printf("Qual tipo de requisição que o novo cliente quer fazer? Digite o número correspondente.\n");
        printf("1. Login (Muito Leve)\n");
        printf("2. HTML (Leve)\n");
        printf("2. Consulta ao Banco de Dados (Média)\n");
        printf("3. Imagem (Pesada)\n");
        printf("5. Vídeo (Muito pesada)\n");

        int tipo_requisicao;
        scanf ("%d", &tipo_requisicao);

        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->id = i;
        args->tipo_req = tipo_requisicao;

        if (pthread_create(&threads[i - 1], NULL, (void*)trata_requisicao, (void*) args) != 0) {
            printf("[ERRO]: Falha ao criar thread para o cliente %d. Encerrando.\n", i);
            free(args);
            return 1;
        }
    }

    for (i = 0; i < num_clientes; i++) {
        pthread_join(threads[i], NULL);
    }

    fim = time(NULL);
    tempo_total = difftime(fim, inicio);

    printf("\n[SERVIDOR]: Todas as requisicoes foram processadas.\n");
    printf("[SERVIDOR]: Tempo total de processamento: %.2f segundos.\n", tempo_total);
    return 0;
}