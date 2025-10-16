#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// --- Definições e Variáveis Globais ---
#define TAMANHO_BUFFER 10
int buffer[TAMANHO_BUFFER];
int in = 0;
int out = 0;

// --- Estruturas de Argumentos ---
typedef struct { int id; int total_a_produzir; } ProdutorArgs;
typedef struct { int id; int total_a_consumir; } ConsumidorArgs;

// --- Protótipos das funções ---
void *produzir(void *arg);
void *consumir(void *arg);
void exibir_buffer();

// Função Auxiliar: Exibe o estado atual do buffer
void exibir_buffer() {
    printf("  [BUFFER]: [");
    for (int i = 0; i < TAMANHO_BUFFER; i++) { printf("%d%s", buffer[i], (i < TAMANHO_BUFFER - 1) ? "," : ""); }
    printf("]\n");
    printf("    -> Inserção (in): %d, Remoção (out): %d\n\n", in, out);
}

// --- Função de Thread Produtora ---
void *produzir(void *arg) {
    ProdutorArgs *args = (ProdutorArgs *)arg;
    int id = args->id;
    int total_a_produzir = args->total_a_produzir;

    for (int i = 0; i < total_a_produzir; i++) {
        printf("[PRODUTOR %d]: STATUS - Produzindo\n", id);
        int in_antes = in;
        printf("[PRODUTOR %d]: Planeja inserir na posição %d.\n", id, in_antes);
        printf("[PRODUTOR %d]: STATUS - Dormindo ...\n", id);
        sleep(1); 
        
        if (in != in_antes) {
            printf("\n\n!!! PROBLEMA DE CONCORRÊNCIA DETECTADO (PRODUTOR %d) !!!\n", id);
            printf(" -> A thread planejava escrever na posição %d, mas o ponteiro 'in' mudou para %d.\n", in_antes, in);
            printf(" -> Ocorrerá uma SOBRESCRITA de dados!\n\n\n");
        }

        printf("[PRODUTOR %d]: Entrou na Região Crítica para escrever.\n", id);
        
        int item_produzido = (rand() % 100) + 1;
        buffer[in_antes] = item_produzido; 
        in = (in_antes + 1) % TAMANHO_BUFFER;

        printf("[PRODUTOR %d]: Saiu da Região Crítica.\n", id);
        
        exibir_buffer();
    }
    free(arg);
    pthread_exit(NULL);
}

// --- Função de Thread Consumidora ---
void *consumir(void *arg) {
    ConsumidorArgs *args = (ConsumidorArgs *)arg;
    int id = args->id;
    int total_a_consumir = args->total_a_consumir;

    for (int i = 0; i < total_a_consumir; i++) {
        printf("[CONSUMIDOR %d]: STATUS - Consumindo\n", id);
        int out_antes = out;
        printf("[CONSUMIDOR %d]: Planeja consumir da posição %d.\n", id, out_antes);
        printf("[CONSUMIDOR %d]: STATUS - Pausando (simulando trabalho)...\n", id);
        sleep(1);
        
        if (out != out_antes) {
            printf("\n\n!!! PROBLEMA DE CONCORRÊNCIA DETECTADO (CONSUMIDOR %d) !!!\n", id);
            printf(" -> A thread planejava ler da posição %d, mas o ponteiro 'out' mudou para %d.\n", out_antes, out);
            printf(" -> Isso pode causar o consumo do MESMO item mais de uma vez!\n\n\n");
        }

        printf("[CONSUMIDOR %d]: Entrou na Região Crítica para ler.\n", id);

        int item_consumido = buffer[out_antes];
        buffer[out_antes] = -1;
        out = (out_antes + 1) % TAMANHO_BUFFER;

        // NOVO: Adicionada a mensagem de saída da Região Crítica
        printf("[CONSUMIDOR %d]: Consumiu item %d da pos %d e Saiu da Região Crítica.\n", id, item_consumido, out_antes);
        
        exibir_buffer();
    }
    free(arg);
    pthread_exit(NULL);
}

// --- Função Principal (main) ---
int main() {
    int num_produtores, num_consumidores, total_produzir_por_thread;
    
    printf("--- Simulação com detecção de problemas de concorrência ---\n");
    printf("Tamanho do Buffer: %d\n", TAMANHO_BUFFER);
    printf("Digite a quantidade de threads Produtoras: ");
    scanf("%d", &num_produtores);
    printf("Digite a quantidade de threads Consumidoras: ");
    scanf("%d", &num_consumidores);
    printf("Digite a quantidade de itens que CADA Produtor irá gerar: ");
    scanf("%d", &total_produzir_por_thread);

    int total_itens_processados = num_produtores * total_produzir_por_thread;
    int total_a_consumir_por_thread = total_itens_processados / num_consumidores;
    
    srand(time(NULL));

    for (int i = 0; i < TAMANHO_BUFFER; i++) { buffer[i] = -1; }

    pthread_t threads[num_produtores + num_consumidores];
    
    printf("\n--- Iniciando simulação ---\n");
    printf("Buffer Inicial:\n");
    exibir_buffer();

    for (int i = 0; i < num_produtores; i++) {
        ProdutorArgs *args = malloc(sizeof(ProdutorArgs));
        args->id = i + 1;
        args->total_a_produzir = total_produzir_por_thread;
        pthread_create(&threads[i], NULL, produzir, (void *)args);
    }

    for (int i = 0; i < num_consumidores; i++) {
        ConsumidorArgs *args = malloc(sizeof(ConsumidorArgs));
        args->id = i + 1;
        args->total_a_consumir = total_a_consumir_por_thread;
        pthread_create(&threads[num_consumidores + i], NULL, consumir, (void *)args);
    }

    for (int i = 0; i < (num_produtores + num_consumidores); i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n--- Simulação finalizada ---\n");
    printf("Total de itens que deveriam ter sido produzidos/consumidos: %d\n", total_itens_processados);
    printf("Compare o valor acima com o estado final do buffer para ver a falha do sistema.\n");
    
    exibir_buffer();

    return 0;
}