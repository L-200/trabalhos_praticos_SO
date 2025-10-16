#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Definições e Variáveis Globais para o buffer e sincronização
#define TAMANHO_BUFFER 10 
int buffer[TAMANHO_BUFFER];
int in = 0;
int out = 0; 
int count = 0; // contador de itens no buffer

pthread_mutex_t mutex;
pthread_cond_t cond_nao_cheio;  //Para produtores esperarem se o buffer estiver cheio
pthread_cond_t cond_nao_vazio; //Para consumidores esperarem se o buffer estiver vazio

// Estruturas de Argumentos (sem alteração)
typedef struct {
    int id;
    int total_a_produzir; 
} ProdutorArgs; 

typedef struct {
    int id;
    int total_a_consumir; 
} ConsumidorArgs; 

void *produzir(void *arg);
void *consumir(void *arg);
void exibir_buffer(); 

void exibir_buffer() { 
    printf("  [BUFFER]: ["); 

    for (int i = 0; i < TAMANHO_BUFFER; i++) {
        printf(" %d", buffer[i]); 
        if (i < TAMANHO_BUFFER - 1) {
            printf(",");
        }
    }
    printf(" ]\n");
    
    printf("           -> Inserção (in): %d\n", in);
    printf("           -> Remoção (out): %d\n", out);
    printf("           -> Itens no buffer (count): %d\n", count);
}


// Função de Thread Produtora
void *produzir(void *arg) {
    ProdutorArgs *args = (ProdutorArgs *)arg;
    int id = args->id;
    int total_a_produzir = args->total_a_produzir; 
    
    for (int i = 0; i < total_a_produzir; i++) {
        int item_produzido = (rand() % 100) + 1; 
        
        // Lógica de espera com mutex e variável de condição
        pthread_mutex_lock(&mutex); 
        printf("[PRODUTOR %d]: Status: Tentando ENTRAR na RC. Verificando se buffer esta cheio.\n", id);
        
        // Enquanto o buffer estiver cheio, espera
        while (count == TAMANHO_BUFFER) {
            printf("[PRODUTOR %d]: Status: Buffer CHEIO. Indo DORMIR (cond_wait).\n", id);
            pthread_cond_wait(&cond_nao_cheio, &mutex);
            printf("[PRODUTOR %d]: Status: ACORDOU. Verificando condicao novamente.\n", id);
        }

        // Região Crítica: Inserção
        buffer[in] = item_produzido;
        printf("[PRODUTOR %d]: Status: PRODUZINDO/Dentro da RC. Inserindo item %d na pos %d\n", id, item_produzido, in);
        sleep(1); // Simula o tempo de produção
        in = (in + 1) % TAMANHO_BUFFER;
        count++; // Incrementa o contador
        
        exibir_buffer(); 
        
        //Sinaliza aos consumidores que o buffer não está mais vazio
        printf("[PRODUTOR %d]: Status: SAINDO da RC. Sinalizando que buffer nao esta vazio (cond_signal).\n", id);
        pthread_cond_signal(&cond_nao_vazio);
        pthread_mutex_unlock(&mutex);
         
    } 

    printf("[PRODUTOR %d]: Status: FINALIZADO.\n", id);
    free(arg);
    pthread_exit(NULL);
} 

// Função de Thread Consumidora
void *consumir(void *arg) {
    ConsumidorArgs *args = (ConsumidorArgs *)arg;
    int id = args->id;
    int total_a_consumir = args->total_a_consumir; 
    int item_consumido; 

    for (int i = 0; i < total_a_consumir; i++) {
        
        // Lógica de espera com mutex e variável de condição
        pthread_mutex_lock(&mutex);
        printf("[CONSUMIDOR %d]: Status: Tentando ENTRAR na RC. Verificando se buffer esta vazio.\n", id);
        
        // Enquanto o buffer estiver vazio, espera
        while (count == 0) {
            printf("[CONSUMIDOR %d]: Status: Buffer VAZIO. Indo DORMIR (cond_wait).\n", id);
            pthread_cond_wait(&cond_nao_vazio, &mutex);
            printf("[CONSUMIDOR %d]: Status: ACORDOU. Verificando condicao novamente.\n", id);
        }

        // Região Crítica: Remoção
        item_consumido = buffer[out];
        buffer[out] = -1; // Marca a posição como vazia
        printf("[CONSUMIDOR %d]: ENTROU na RC. Consumiu item %d na pos %d\n", id, item_consumido, out);
        sleep(1); // Simula o tempo de consumo
        out = (out + 1) % TAMANHO_BUFFER; 
        count--; // Decrementa o contador
        
        exibir_buffer(); 
        
        // Sinaliza aos produtores que o buffer não está mais cheio
        printf("[CONSUMIDOR %d]: Status: SAINDO da RC. Sinalizando que buffer nao esta cheio (cond_signal).\n", id);
        pthread_cond_signal(&cond_nao_cheio);
        pthread_mutex_unlock(&mutex);

    } 

    printf("[CONSUMIDOR %d]: Status: FINALIZADO.\n", id);
    free(arg);
    pthread_exit(NULL);
} 

// Função Principal (main)
int main() {
    int num_produtores, total_produzir_por_thread, total_consumir;
    int num_consumidores = 1; 

    printf("--- Simulacao Produtores - 1 Consumidor (com Mutex + Var. de Condicao) ---\n");
    printf("Tamanho do Buffer: %d\n", TAMANHO_BUFFER); 

    printf("Digite a quantidade de threads Produtoras: ");
    scanf("%d", &num_produtores); 

    printf("Digite a quantidade de itens que CADA Produtor ira gerar: ");
    scanf("%d", &total_produzir_por_thread); 
    
    total_consumir = num_produtores * total_produzir_por_thread;
    printf("\nTotal de itens a ser consumido (por 1 consumidor): %d\n", total_consumir);
    
    srand(time(NULL)); 

    // Inicializa o mutex e as variáveis de condição
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_nao_cheio, NULL);
    pthread_cond_init(&cond_nao_vazio, NULL);

    for (int i = 0; i < TAMANHO_BUFFER; i++) {
        buffer[i] = -1; 
    } 

    pthread_t threads[num_produtores + num_consumidores]; 

    printf("\n--- Iniciando simulacao ---\n");
    printf("Buffer Inicial:\n");
    exibir_buffer(); 

    for (int i = 0; i < num_consumidores; i++) {
        ConsumidorArgs *args = malloc(sizeof(ConsumidorArgs));
        args->id = i + 1;
        args->total_a_consumir = total_consumir;
        pthread_create(&threads[i], NULL, consumir, (void *)args);
    }
    
    for (int i = 0; i < num_produtores; i++) {
        ProdutorArgs *args = malloc(sizeof(ProdutorArgs));
        args->id = i + 1;
        args->total_a_produzir = total_produzir_por_thread;
        pthread_create(&threads[num_consumidores + i], NULL, produzir, (void *)args);
    } 

    for (int i = 0; i < (num_produtores + num_consumidores); i++) {
        pthread_join(threads[i], NULL);
    } 

    // Destroi os recursos de sincronização
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_nao_cheio);
    pthread_cond_destroy(&cond_nao_vazio);

    printf("\n--- Simulacao finalizada ---\n");
    printf("Total de itens produzidos/consumidos: %d\n", total_consumir);
    printf("\n");

    return 0;
}