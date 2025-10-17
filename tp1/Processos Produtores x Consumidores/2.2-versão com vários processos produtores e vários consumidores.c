#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h> 

// Definições e Variáveis Globais para o buffer e sincronização
#define TAMANHO_BUFFER 10 
int buffer[TAMANHO_BUFFER];
int in = 0;
int out = 0; 

pthread_mutex_t mutex;
sem_t full;
sem_t empty; 

// Estruturas de Argumentos
typedef struct {
    int id;
    int total_a_produzir; 
} ProdutorArgs; 

typedef struct {
    int id;
    int total_a_consumir; 
} ConsumidorArgs; 

// Protótipos das funções
void *produzir(void *arg);
void *consumir(void *arg);
void exibir_buffer(); 

// Função Auxiliar: Exibe o estado atual do buffer
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
}


// Função de Thread Produtora
void *produzir(void *arg) {
    ProdutorArgs *args = (ProdutorArgs *)arg;
    int id = args->id;
    int total_a_produzir = args->total_a_produzir; 
    
    for (int i = 0; i < total_a_produzir; i++) {
        int item_produzido = (rand() % 100) + 1; 
        
        // Status: Verificando e podendo bloquear (dormir)
        printf("[PRODUTOR %d]: Status: Verificando espaco (sem_empty). Pode ir DORMIR.\n", id);
        sem_wait(&empty); 
        
        // Status: Acordou e tenta acesso exclusivo
        printf("[PRODUTOR %d]: Status: ACORDOU/Prosseguiu. Tentando ENTRAR na RC.\n", id);
        pthread_mutex_lock(&mutex); 

        // Região Crítica: Inserção
        buffer[in] = item_produzido;
        sleep(1); 
        printf("[PRODUTOR %d]: Status: PRODUZINDO/Dentro da RC. Inserindo item %d na pos %d\n", id, item_produzido, in);
        in = (in + 1) % TAMANHO_BUFFER;
        
        exibir_buffer(); 

        // Status: Saindo da Região Crítica
        printf("[PRODUTOR %d]: Status: SAINDO da RC. Sinalizando item cheio.\n", id);
        pthread_mutex_unlock(&mutex);
        
        // Sinaliza um item cheio
        sem_post(&full); 
    } 

   // Status: Finalizando
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
        
        // Status: Verificando e podendo bloquear (dormir)
        printf("[CONSUMIDOR %d]: Status: Verificando item (sem_full). Pode ir DORMIR.\n", id);
        sem_wait(&full); 
        
        // Status: Acordou e tenta acesso exclusivo
        printf("[CONSUMIDOR %d]: Status: ACORDOU/Prosseguiu. Tentando ENTRAR na RC.\n", id);
        pthread_mutex_lock(&mutex); 

        // Região Crítica: Remoção
        item_consumido = buffer[out];
        buffer[out] = -1; // Marca a posição como vazia
        sleep(1); 
        printf("[CONSUMIDOR %d]: ENTROU na RC. Consumiu item %d na pos %d\n", id, item_consumido, out);
        out = (out + 1) % TAMANHO_BUFFER; 
        
        exibir_buffer(); 

        // Status: Saindo da Região Crítica
        printf("[CONSUMIDOR %d]: Status: SAINDO da RC. Sinalizando espaço vazio.\n", id);
        pthread_mutex_unlock(&mutex);
        
        // Sinaliza um espaço vazio
        sem_post(&empty); 
    } 

    // Status: Finalizando
    printf("[CONSUMIDOR %d]: Status: FINALIZADO.\n", id);
    free(arg);
    pthread_exit(NULL);
} 

// Função Principal (main) 

int main() {
    int num_produtores, num_consumidores, total_produzir_por_thread, total_produzir, total_consumir; 

    printf("--- Simulacao Produtores - Consumidores ---\n");
    printf("Tamanho do Buffer: %d\n", TAMANHO_BUFFER); 

    printf("Digite a quantidade de threads Produtoras: ");
    scanf("%d", &num_produtores); 

printf("Digite a quantidade de threads Consumidoras: ");
    scanf("%d", &num_consumidores); 

    printf("Digite a quantidade de itens que CADA Produtor ira gerar: ");
    scanf("%d", &total_produzir_por_thread); 

    // Calcula o total de itens que o único consumidor deve processar 

    total_produzir = num_produtores * total_produzir_por_thread; 

    //Calcula o total por thread consumidora

    total_consumir = total_produzir / num_consumidores; 


    printf("\nTotal de itens a ser produzido: %d\n", total_produzir);
    printf("\nTotal de itens a ser consumido (por 1 consumidor): %d\n", total_consumir);
    
    srand(time(NULL)); 

    // Inicializa o mutex e os semáforos
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0); 
    sem_init(&empty, 0, TAMANHO_BUFFER); 

    // Inicializa o buffer com -1 para visualização
    for (int i = 0; i < TAMANHO_BUFFER; i++) {
        buffer[i] = -1; 
    } 

    pthread_t threads[num_produtores + num_consumidores]; 

    printf("\n--- Iniciando simulacao ---\n");
    printf("Buffer Inicial:\n");
    exibir_buffer(); 

    // Cria as threads Consumidoras
    for (int i = 0; i < num_consumidores; i++) {
        ConsumidorArgs *args = malloc(sizeof(ConsumidorArgs));
        args->id = i + 1;
        args->total_a_consumir = total_consumir;
        pthread_create(&threads[i], NULL, consumir, (void *)args);
    }
    
    // Cria as threads Produtoras
    for (int i = 0; i < num_produtores; i++) {
        ProdutorArgs *args = malloc(sizeof(ProdutorArgs));
        args->id = i + 1;
        args->total_a_produzir = total_produzir_por_thread;
        pthread_create(&threads[num_consumidores + i], NULL, produzir, (void *)args);
    } 

    // Espera que todas as threads terminem
    for (int i = 0; i < (num_produtores + num_consumidores); i++) {
        pthread_join(threads[i], NULL);
    } 

    // Destroi os recursos de sincronização
    pthread_mutex_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty); 

    printf("\n--- Simulacao finalizada ---\n");
    printf("Total de itens produzidos/consumidos: %d\n", total_consumir);
    printf("\n");

    return 0;
}
