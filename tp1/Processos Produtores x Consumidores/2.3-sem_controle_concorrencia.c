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

// MUDANÇA: Contadores globais para auditoria final (também desprotegidos)
int g_itens_produzidos_total = 0;
int g_itens_consumidos_total = 0;
int g_dados_sobrescritos = 0;

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
    for (int i = 0; i < TAMANHO_BUFFER; i++) { printf(" %2d%s", buffer[i], (i < TAMANHO_BUFFER - 1) ? "," : ""); }
    printf(" ]\n");
    printf("    -> Inserção (in): %d, Remoção (out): %d\n\n", in, out);
}

// --- Função de Thread Produtora ---
void *produzir(void *arg) {
    ProdutorArgs *args = (ProdutorArgs *)arg;
    int total_a_produzir = args->total_a_produzir;

    for (int i = 0; i < total_a_produzir; i++) {
        // MUDANÇA: A thread "pega" o ponteiro 'in' atual. Isso é o início da condição de corrida.
        int posicao_alvo = in;

        // A pausa aumenta a chance de outra thread alterar 'in' enquanto esta dorme.
        sleep(1);

        // Gera o item a ser produzido
        int item_produzido = (rand() % 100) + 1;

        // DETECÇÃO DE PROBLEMA: Verifica se o local no buffer já está ocupado por um item válido
        if (buffer[posicao_alvo] != -1) {
            printf("!!! SOBRESCRITA DETECTADA: Posição %d já continha %d, será sobrescrito por %d !!!\n",
                   posicao_alvo, buffer[posicao_alvo], item_produzido);
            g_dados_sobrescritos++;
        }

        // Escreve no buffer usando a cópia local do ponteiro. Isso evita o segfault.
        buffer[posicao_alvo] = item_produzido;
        printf("[PRODUTOR %d]: Inseriu item %d na posição %d.\n", args->id, item_produzido, posicao_alvo);

        // Atualiza o ponteiro global de forma insegura.
        in = (posicao_alvo + 1) % TAMANHO_BUFFER;
        
        g_itens_produzidos_total++; // Esta operação (leitura-modificação-escrita) também tem race condition!
    }
    free(arg);
    pthread_exit(NULL);
}

// --- Função de Thread Consumidora ---
void *consumir(void *arg) {
    ConsumidorArgs *args = (ConsumidorArgs *)arg;
    int total_a_consumir = args->total_a_consumir;

    for (int i = 0; i < total_a_consumir; i++) {
        // A thread "pega" o ponteiro 'out' atual.
        int posicao_alvo = out;

        sleep(1);

        int item_consumido = buffer[posicao_alvo];

        // Se o item for -1, significa que outro consumidor já limpou essa posição
        // ou um produtor ainda não escreveu lá. Em ambos os casos, é um erro de consumo.
        if (item_consumido == -1) {
            printf("!!! CONSUMO INVÁLIDO DETECTADO: [CONSUMIDOR %d] tentou ler da posição %d que já estava vazia!!!\n",
                   args->id, posicao_alvo);
        } else {
            printf("[CONSUMIDOR %d]: Consumiu item %d da posição %d.\n", args->id, item_consumido, posicao_alvo);
        }
        
        buffer[posicao_alvo] = -1; // Marca como vazio

        // Atualiza o ponteiro global de forma insegura.
        out = (posicao_alvo + 1) % TAMANHO_BUFFER;
        
        g_itens_consumidos_total++; // Race condition aqui também!
    }
    free(arg);
    pthread_exit(NULL);
}

// --- Função Principal (main) ---
int main() {
    int num_produtores, num_consumidores, total_produzir_por_thread;
    
    printf("--- Simulação de Falha Lógica (sem crash) ---\n");
    printf("Digite a quantidade de threads Produtoras: ");
    scanf("%d", &num_produtores);
    printf("Digite a quantidade de threads Consumidoras: ");
    scanf("%d", &num_consumidores);
    printf("Digite a quantidade de itens que CADA Produtor irá gerar: ");
    scanf("%d", &total_produzir_por_thread);

    int total_itens_esperados = num_produtores * total_produzir_por_thread;
    int total_a_consumir_por_thread = total_itens_esperados / num_consumidores;
    
    srand(time(NULL));

    for (int i = 0; i < TAMANHO_BUFFER; i++) { buffer[i] = -1; }

    pthread_t threads[num_produtores + num_consumidores];
    
    printf("\n--- Iniciando simulação ---\n");

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
        pthread_create(&threads[num_produtores + i], NULL, consumir, (void *)args);
    }

    for (int i = 0; i < (num_produtores + num_consumidores); i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n\n--- Relatório Final de Corrupção ---\n");
    printf("Total de itens que deveriam ter sido produzidos: %d\n", total_itens_esperados);
    printf("Contador global de itens produzidos (incorreto devido à concorrência): %d\n", g_itens_produzidos_total);
    printf("Contador global de itens consumidos (incorreto): %d\n\n", g_itens_consumidos_total);
    printf("Número de vezes que um dado foi explicitamente SOBRESCRITO: %d\n", g_dados_sobrescritos);
    
    int itens_restantes_no_buffer = 0;
    for (int i = 0; i < TAMANHO_BUFFER; i++) {
        if (buffer[i] != -1) {
            itens_restantes_no_buffer++;
        }
    }
    printf("Itens que restaram no buffer: %d\n", itens_restantes_no_buffer);
    printf("Total de itens perdidos (Esperado - Restantes): %d\n", total_itens_esperados - itens_restantes_no_buffer);

    printf("\nEstado final do buffer:\n");
    exibir_buffer();

    return 0;
}