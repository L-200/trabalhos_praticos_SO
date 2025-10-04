#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Estrutura para a variável compartilhada
typedef struct {
    int quantidade;
} EstoqueProduto;

// Estrutura para passar argumentos para as threads de venda
typedef struct {
    int id;
    int total_a_vender; 
} VendaArgs;

EstoqueProduto estoque_compartilhado;

// NENHUM MUTEX É DECLARADO NESTA VERSÃO

void *realizar_venda(void *arg) {
    VendaArgs *args = (VendaArgs *)arg;

    int id = args->id;
    int total_vendas = args->total_a_vender;

    printf("[VENDA %d]: Thread iniciada. Tentará vender %d itens.\n", id, total_vendas);

    // --- INÍCIO DA REGIÃO CRÍTICA (SEM PROTEÇÃO) ---

    // 1. Lê o valor atual do estoque para uma variável local.
    // Várias threads podem ler o MESMO valor aqui antes que qualquer uma delas o atualize.
    int estoque_no_momento_da_leitura = estoque_compartilhado.quantidade;
    printf("[VENDA %d]: Leu o estoque. Valor: %d\n", id, estoque_no_momento_da_leitura);
    
    if (estoque_no_momento_da_leitura >= total_vendas) {
        printf("[VENDA %d]: Estoque suficiente. Processando a baixa...\n", id);

        // 2. Dorme por um curto período para aumentar a chance de outra thread executar
        // e ler o valor antigo do estoque, criando uma condição de corrida.
        sleep(2); 
        
        // 3. Atualiza o estoque com base no valor lido anteriormente (que pode estar desatualizado).
        estoque_compartilhado.quantidade = estoque_no_momento_da_leitura - total_vendas; 
        
        printf("[VENDA %d]: Venda de %d itens CONCLUÍDA. Estoque DEVERIA ser atualizado para: %d\n", id, total_vendas, estoque_compartilhado.quantidade);
        
    } else {
        printf("[VENDA %d]: FALHA. Estoque no momento da leitura (%d) era insuficiente para %d vendas.\n", id, estoque_no_momento_da_leitura, total_vendas);
    }
    
    // --- FIM DA REGIÃO CRÍTICA (SEM PROTEÇÃO) ---

    free(arg);
    pthread_exit(NULL);
}


int main() {
    printf("Cenário: Múltiplas vendas (escritores) sem NENHUM controle de concorrência.\n");
    printf("Isto irá demonstrar uma CONDIÇÃO DE CORRIDA (race condition).\n");
    printf("------------------------------------------------------------------\n");

    int num_vendas, total_vendas_por_thread;

    printf("Digite a quantidade de produtos no estoque inicial: ");
    scanf("%d", &estoque_compartilhado.quantidade);

    printf("Digite a quantidade de threads de Venda (escritoras): ");
    scanf("%d", &num_vendas);

    printf("Digite a quantidade de itens que CADA vendedor irá vender: ");
    scanf("%d", &total_vendas_por_thread);

    int estoque_inicial = estoque_compartilhado.quantidade;

    // NENHUM MUTEX É INICIALIZADO

    pthread_t threads_venda[num_vendas];
    printf("\n--- Iniciando simulação ---\n");
    printf("Estoque inicial: %d\n\n", estoque_compartilhado.quantidade);

    // Criando apenas as threads de venda (escritores)
    for (int i = 0; i < num_vendas; i++) {
        VendaArgs *args = malloc(sizeof(VendaArgs));
        args->id = i + 1;
        args->total_a_vender = total_vendas_por_thread;
        pthread_create(&threads_venda[i], NULL, realizar_venda, (void *)args);
    }

    for (int i = 0; i < num_vendas; i++) {
        pthread_join(threads_venda[i], NULL);
    }

    // NENHUM MUTEX É DESTRUÍDO

    int estoque_esperado = estoque_inicial - (num_vendas * total_vendas_por_thread);

    printf("\n--- Simulação finalizada ---\n");
    printf("Estoque inicial era: %d\n", estoque_inicial);
    printf("Estoque final real: %d\n", estoque_compartilhado.quantidade);
    printf("Estoque final esperado: %d\n", estoque_esperado);
    printf("\n");
    printf("Se o estoque final real for diferente do esperado, a condição de corrida ocorreu e vendas foram 'perdidas'.\n");
 
    return 0;
}