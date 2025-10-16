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
    printf("    -> Inserção (in): %d, Remoção (out): %d\n", in, out);
}

// --- Função de Thread Produtora ---
void *produzir(void *arg) {
    ProdutorArgs *args = (ProdutorArgs *)arg;
    int total_a_produzir = args->total_a_produzir;

    for (int i = 0; i < total_a_produzir; i++) {
        // MENSAGEM DE STATUS: Entrando na Região Crítica
        printf("\n[PRODUTOR %d]: Status: ATIVO. >>> TENTANDO ENTRAR NA REGIÃO CRÍTICA <<<\n", args->id);

        int posicao_alvo = in;
        printf("[PRODUTOR %d]: Planejando inserir na posição %d. Estado ATUAL do buffer:\n", args->id, posicao_alvo);
        exibir_buffer();

        // MENSAGEM DE STATUS: Dormindo
        printf("[PRODUTOR %d]: Status: DORMINDO (simulando I/O ou processamento demorado).\n", args->id);
        sleep(1);
        printf("[PRODUTOR %d]: Status: ACORDOU. Retomando produção.\n", args->id);

        int item_produzido = (rand() % 100) + 1;

        if (buffer[posicao_alvo] != -1) {
            printf("!!! SOBRESCRITA DETECTADA: Posição %d já continha %d, será sobrescrito por %d !!!\n",
                   posicao_alvo, buffer[posicao_alvo], item_produzido);
            g_dados_sobrescritos++;
        }

        buffer[posicao_alvo] = item_produzido;
        
        // MENSAGEM DE STATUS: Produzindo
        printf("[PRODUTOR %d]: Status: PRODUZINDO. Inseriu item %d na posição %d. Estado NOVO do buffer:\n", args->id, item_produzido, posicao_alvo);
        exibir_buffer();

        in = (posicao_alvo + 1) % TAMANHO_BUFFER;
        g_itens_produzidos_total++;

        // MENSAGEM DE STATUS: Saindo da Região Crítica
        printf("[PRODUTOR %d]: Status: FINALIZADO. <<< SAINDO DA REGIÃO CRÍTICA >>>\n", args->id);
    }
    free(arg);
    pthread_exit(NULL);
}

// --- Função de Thread Consumidora ---
void *consumir(void *arg) {
    ConsumidorArgs *args = (ConsumidorArgs *)arg;
    int total_a_consumir = args->total_a_consumir;

    for (int i = 0; i < total_a_consumir; i++) {
        // MENSAGEM DE STATUS: Entrando na Região Crítica
        printf("\n[CONSUMIDOR %d]: Status: ATIVO. >>> TENTANDO ENTRAR NA REGIÃO CRÍTICA <<<\n", args->id);
        
        int posicao_alvo = out;
        printf("[CONSUMIDOR %d]: Planejando consumir da posição %d. Estado ATUAL do buffer:\n", args->id, posicao_alvo);
        exibir_buffer();

        // MENSAGEM DE STATUS: Dormindo
        printf("[CONSUMIDOR %d]: Status: DORMINDO (aguardando item ou simulando processamento).\n", args->id);
        sleep(1);
        printf("[CONSUMIDOR %d]: Status: ACORDOU. Retomando consumo.\n", args->id);

        int item_consumido = buffer[posicao_alvo];

        if (item_consumido == -1) {
            printf("!!! CONSUMO INVÁLIDO DETECTADO: [CONSUMIDOR %d] tentou ler da posição %d que já estava vazia!!!\n",
                   args->id, posicao_alvo);
        } else {
            // MENSAGEM DE STATUS: Consumindo
            printf("[CONSUMIDOR %d]: Status: CONSUMINDO. Consumiu item %d da posição %d.\n", args->id, item_consumido, posicao_alvo);
        }
        
        buffer[posicao_alvo] = -1;
        printf("[CONSUMIDOR %d]: Finalizou consumo na posição %d. Estado NOVO do buffer:\n", args->id, posicao_alvo);
        exibir_buffer();
        
        out = (posicao_alvo + 1) % TAMANHO_BUFFER;
        g_itens_consumidos_total++;

        // MENSAGEM DE STATUS: Saindo da Região Crítica
        printf("[CONSUMIDOR %d]: Status: FINALIZADO. <<< SAINDO DA REGIÃO CRÍTICA >>>\n", args->id);
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
    // Garante que pelo menos um item seja consumido por thread se possível
    int total_a_consumir_por_thread = (num_consumidores > 0) ? (total_itens_esperados / num_consumidores) : 0;
    
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
    printf("Total de itens perdidos (Esperado - Consumidos - Restantes): %d\n", total_itens_esperados - (g_itens_consumidos_total - g_dados_sobrescritos) - itens_restantes_no_buffer);

    printf("\nEstado final do buffer:\n");
    exibir_buffer();

    return 0;
}