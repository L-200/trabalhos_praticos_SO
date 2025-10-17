#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// struct para a variável compartilhada
typedef struct {
    int quantidade;
} EstoqueProduto;

// struct para passar argumentos para as threads de venda
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

    //região crítica SEM PROTEÇÃO (SEM MUTEX)
    printf("[VENDA %d]: Entrou na região crítica SEM PROTEÇÃO (SEM MUTEX).\n", id);
    int estoque_no_momento_da_leitura = estoque_compartilhado.quantidade;
    printf("[VENDA %d]: Leu o estoque. Valor: %d\n", id, estoque_no_momento_da_leitura);
    
    if (estoque_no_momento_da_leitura >= total_vendas) {
        printf("    [VENDA %d]: Estoque suficiente. Processando a baixa...\n", id);
        printf("    [VENDA %d]: QUANTIDADE APÓS A VENDA DEVERIA SER: %d\n", id, estoque_no_momento_da_leitura - total_vendas);
        printf("    [VENDA %d]: DORMINDO, race condition provavelmente ocorrerá agora\n", id);
        // a thread dorme depois de verificar o estoque para simular um atraso na venda
        sleep(1); // simula o tempo de processamento da venda
        
        estoque_compartilhado.quantidade = estoque_no_momento_da_leitura - total_vendas; 
        printf("    [VENDA %d]: Venda de %d itens CONCLUÍDA. Estoque atualizado para: %d\n", id, total_vendas, estoque_compartilhado.quantidade);
        
    } else {
        printf("[VENDA %d]: FALHA. Estoque no momento da leitura (%d) era insuficiente para %d vendas.\n", id, estoque_no_momento_da_leitura, total_vendas);
    }
    
    printf("[VENDA %d]: Saindo da região crítica SEM PROTEÇÃO (SEM MUTEX).\n", id);
    // fim da região crítica SEM PROTEÇÃO (SEM MUTEX)

    free(arg);
    pthread_exit(NULL);
}


int main() {
    printf("Tema: e-commerce\n");
    printf("Cenário: múltiplas thread de vendas (escritoras) sem nenhum controle de concorrência.\n");
    printf("Isto irá demonstrar uma CONDIÇÃO DE CORRIDA (race condition).\n");
    printf("------------------------------------------------------------------\n");
    srand(time(NULL));

    int num_vendas, total_vendas_por_thread;

    printf("Digite a quantidade de produtos no estoque inicial: ");
    scanf("%d", &estoque_compartilhado.quantidade);

    printf("Digite a quantidade de threads de Venda (use cerca de 10 para produzir resultados menos monotonos): ");
    scanf("%d", &num_vendas);

    printf("Digite a quantidade de itens que CADA vendedor irá vender: ");
    scanf("%d", &total_vendas_por_thread);

    int estoque_inicial = estoque_compartilhado.quantidade;

    pthread_t threads_venda[num_vendas];
    printf("\n--- Iniciando simulação ---\n");
    printf("Estoque inicial: %d\n\n", estoque_compartilhado.quantidade);

    //criando apenas as threads de venda (escritores)
    for (int i = 0; i < num_vendas; i++) {
        VendaArgs *args = malloc(sizeof(VendaArgs));
        args->id = i + 1;
        args->total_a_vender = total_vendas_por_thread;
        pthread_create(&threads_venda[i], NULL, realizar_venda, (void *)args);
        usleep((rand() % 1000000) + 10000); // pequena pausa para tentar embaralhar a ordem de execução das threads e produzir resultados mais interessantes
    }

    for (int i = 0; i < num_vendas; i++) {
        pthread_join(threads_venda[i], NULL);
    }

    int estoque_esperado = estoque_inicial - (num_vendas * total_vendas_por_thread);

    printf("\n--- Simulação finalizada ---\n");
    printf("Estoque inicial era: %d\n", estoque_inicial);
    printf("Estoque final: %d\n", estoque_compartilhado.quantidade);
    printf("Estoque final esperado: %d\n", estoque_esperado);
    printf("\n");
    printf("Se o estoque final real for diferente do esperado, a condição de corrida ocorreu e vendas foram 'perdidas'.\n");
 
    return 0;
}