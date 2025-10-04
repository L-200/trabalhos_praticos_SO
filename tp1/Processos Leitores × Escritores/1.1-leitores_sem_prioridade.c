#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// estrutura para a variável compartilhada
typedef struct {
    int quantidade;
} EstoqueProduto;

// estrutura para passar argumentos para as threads de venda
typedef struct {
    int id;
    int total_a_vender; 
} VendaArgs;

// estrutura para passar argumentos para as threads de consulta
typedef struct {
    int id;
    int num_leituras; 
} ConsultaArgs;

EstoqueProduto estoque_compartilhado;
pthread_mutex_t mutex_venda; // mutex usado apenas pelas vendas

void *realizar_venda(void *arg) {
    VendaArgs *args = (VendaArgs *)arg;

    int id = args->id;
    int total_vendas = args->total_a_vender;

    printf("[VENDA %d]: Thread iniciada. Tentará vender %d itens.\n", id, total_vendas);

    // tenta adquirir o lock
    printf("[VENDA %d]: Aguardando para entrar na região crítica...\n", id);
    pthread_mutex_lock(&mutex_venda);

    // inicio da região crítica
    printf("\n[VENDA %d]: ENTROU NA REGIÃO CRÍTICA. O estoque neste momento é: %d\n", id, estoque_compartilhado.quantidade);
    
    if (estoque_compartilhado.quantidade >= total_vendas) {
        printf("[VENDA %d]: Estoque suficiente. Processando a venda...\n", id);
        printf("[VENDA %d]: QUANTIDADE APÓS A VENDA: %d\n", id, estoque_compartilhado.quantidade - total_vendas);

        // a thread dorme depois de verificar o estoque para simular um atraso na venda
        // as threads de consulta que executarem agora vãp ler um valor sujo
        printf("[VENDA %d]: DORMINDO por 2 segundos ANTES de efetivar a baixa no estoque. Leitores podem ver valores antigos agora!\n", id);
        sleep(1); 

        estoque_compartilhado.quantidade -= total_vendas; 
        
        printf("\n[VENDA %d]: Venda de %d itens CONCLUÍDA. Estoque atualizado para: %d\n", id, total_vendas, estoque_compartilhado.quantidade);
        
    } else {
        printf("[VENDA %d]: FALHA. Estoque inicial (%d) insuficiente para processar %d vendas.\n", id, estoque_compartilhado.quantidade, total_vendas);
    }
    
    printf("[VENDA %d]: SAINDO da sua região crítica.\n\n", id);
    //  fim da região crítica
    pthread_mutex_unlock(&mutex_venda);

    free(arg);
    pthread_exit(NULL);
}

void *consultar_estoque(void *arg) {
    ConsultaArgs *args = (ConsultaArgs *)arg;
    int id = args->id;
    int num_leituras = args->num_leituras;
    int estoque_lido;

    sleep(2); 

    printf("[CONSULTA %d]: Iniciada. Fará %d leituras para monitorar o estoque ao longo da simulaçãp.\n", id, num_leituras);

    for (int i = 0; i < num_leituras; i++) {
        estoque_lido = estoque_compartilhado.quantidade;
        printf("    [CONSULTA %d]: Estoque lido: %d\n", id, estoque_lido);
        sleep(1);
    }

    printf("[CONSULTA %d]: Finalizada.\n", id);
    free(arg);
    pthread_exit(NULL);
}


int main() {
    printf("Cenário: e-commerce onde as vendas estão protegidas pelo mutex mas as consultas não.\n");
    printf("-------------\n");

    int num_consultas, num_vendas, total_vendas_por_thread;

    printf("Digite a quantidade de produtos no estoque inicial: ");
    scanf("%d", &estoque_compartilhado.quantidade);

    printf("Digite a quantidade de threads de Consulta (leitoras): ");
    scanf("%d", &num_consultas);

    printf("Digite a quantidade de threads de Venda (escritoras): ");
    scanf("%d", &num_vendas);

    printf("Digite a quantidade de itens que CADA vendedor irá vender: ");
    scanf("%d", &total_vendas_por_thread);

    int estoque_inicial = estoque_compartilhado.quantidade;
    int leituras_por_thread = num_vendas;

    pthread_mutex_init(&mutex_venda, NULL);
    srand(time(NULL));

    pthread_t threads[num_consultas + num_vendas];
    printf("\n--- Iniciando simulação ---\n");
    printf("Estoque inicial: %d\n\n", estoque_compartilhado.quantidade);

    // criando as threads de consulta (leitores SEM mutex)
   for (int i = 0; i < num_consultas; i++) {
        ConsultaArgs *args = malloc(sizeof(ConsultaArgs));
        args->id = i + 1;
        args->num_leituras = leituras_por_thread;
        pthread_create(&threads[i], NULL, consultar_estoque, (void *)args);
    }

    // criando as threads de venda (escritores COM mutex)
    for (int i = 0; i < num_vendas; i++) {
        VendaArgs *args = malloc(sizeof(VendaArgs));
        args->id = i + 1;
        args->total_a_vender = total_vendas_por_thread;
        pthread_create(&threads[num_consultas + i], NULL, realizar_venda, (void *)args);
    }

    for (int i = 0; i < (num_consultas + num_vendas); i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex_venda);

    printf("\n--- Simulação finalizada ---\n");
    printf("Estoque inicial: %d\n", estoque_inicial);
    printf("Estoque final: %d\n", estoque_compartilhado.quantidade);
    printf("\n");
    printf("Observe se alguma thread de consulta leu o valor errado após uma thread de venda marcar que ele (o valor) seria mudado após uma venda entrar em sua região crítica.\n");
 
    return 0;
}