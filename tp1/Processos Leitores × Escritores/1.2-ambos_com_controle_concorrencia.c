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
    int id_fake;
    int total_a_vender; 
} VendaArgs;

// estrutura para passar argumentos para as threads de consulta
typedef struct {
    int id;
    int id_fake;
} ConsultaArgs;

EstoqueProduto estoque_compartilhado;
pthread_mutex_t mutex_venda_consulta; // mutex usado apenas pelas vendas e pelas consultas

void *realizar_venda(void *arg) {
    VendaArgs *args = (VendaArgs *)arg;

    usleep((rand() % 100000) + 10000); //pequena pausa para embaralhar a ordem de execução das threads

    int id = args->id;
    int id_fake = args->id_fake;
    int total_vendas = args->total_a_vender;

    printf("[VENDA %d]: Thread iniciada. Tentará vender %d itens.\n", id_fake, total_vendas);

    // tenta adquirir o lock
    printf("[VENDA %d]: Aguardando para entrar na região crítica...\n", id_fake);
    pthread_mutex_lock(&mutex_venda_consulta);

    // inicio da região crítica
    printf("\n[VENDA %d]: ENTROU NA REGIÃO CRÍTICA. O estoque neste momento é: %d\n", id_fake, estoque_compartilhado.quantidade);
    
    if (estoque_compartilhado.quantidade >= total_vendas) {
        printf("    [VENDA %d]: Estoque suficiente. Processando a venda...\n", id_fake);
        printf("    [VENDA %d]: QUANTIDADE APÓS A VENDA: %d\n", id_fake, estoque_compartilhado.quantidade - total_vendas);

        // a thread dorme depois de verificar o estoque para simular um atraso na venda
        // as threads de consulta que executarem agora vãp ler um valor sujo
        printf("    [VENDA %d]: DORMINDO por 2 segundos ANTES de efetivar a baixa no estoque. Leitores poderiam ver valores antigos agora se não tivesse o mutex!\n", id);
        sleep(1); 

        estoque_compartilhado.quantidade -= total_vendas; 
        
        printf("\n    [VENDA %d]: Venda de %d itens CONCLUÍDA. Estoque atualizado para: %d\n", id_fake, total_vendas, estoque_compartilhado.quantidade);
        
    } else {
        printf("[VENDA %d]: FALHA. Estoque inicial (%d) insuficiente para processar %d vendas.\n", id_fake, estoque_compartilhado.quantidade, total_vendas);
    }
    
    printf("[VENDA %d]: SAINDO da sua região crítica.\n\n", id_fake);
    //  fim da região crítica
    pthread_mutex_unlock(&mutex_venda_consulta);

    free(arg);
    pthread_exit(NULL);
}

void *consultar_estoque(void *arg) {
    ConsultaArgs *args = (ConsultaArgs *)arg;

    usleep((rand() % 100000) + 10000); //pequena pausa para embaralhar a ordem de execução das threads

    int id_fake = args->id_fake;
    int id = args->id;
    int estoque_lido;

    printf("[CONSULTA %d]: Iniciada. Tentará consultar a quantidade disponível no estoque.\n", id_fake);

    
    // tenta adquirir o lock
    printf("[CONSULTA %d]: Aguardando para entrar na região crítica...\n", id_fake);
    pthread_mutex_lock(&mutex_venda_consulta);

    // inicio da região crítica
    printf("\n[CONSULTA %d]: ENTROU NA REGIÃO CRÍTICA.\n", id_fake);
    printf("    [CONSULTA %d]: Lendo o estoque...\n", id_fake);
    estoque_lido = estoque_compartilhado.quantidade;
    sleep(1); //simula o tempo de leitura
    printf("    [CONSULTA %d]: Estoque lido: %d\n", id_fake, estoque_lido);
    printf("[CONSULTA %d]: SAINDO da sua região crítica.\n\n", id_fake);
    // fim da região crítica

    pthread_mutex_unlock(&mutex_venda_consulta);

    printf("[CONSULTA %d]: Finalizada.\n", id_fake);
    free(arg);
    pthread_exit(NULL);
}


// VERSÃO COM CRIAÇÃO INTERCALADA E VETOR ÚNICO
int main() {
    printf("Cenário: e-commerce onde as vendas e as consultas do estoque estão protegidas pelo mutex.\n");
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

    pthread_mutex_init(&mutex_venda_consulta, NULL);
    srand(time(NULL));

    // vetor unico para armazenar todas as threads
    pthread_t threads[num_consultas + num_vendas];
    int total_threads = num_consultas + num_vendas;

    printf("\n--- Iniciando simulação ---\n");
    printf("Estoque inicial: %d\n\n", estoque_compartilhado.quantidade);

    int vendas_criadas = 0;
    int consultas_criadas = 0;
    int thread_idx = 0; // indice mestre para o vetor único

    // loop que continua enquanto houver threads de qualquer tipo para criar
    while (vendas_criadas < num_vendas || consultas_criadas < num_consultas) {
        
        // tenta criar uma thread de VENDA se ainda houver alguma para criar
        if (vendas_criadas < num_vendas) {
            VendaArgs *args = malloc(sizeof(VendaArgs));
            if (!args) {
                perror("Falha ao alocar memória para VendaArgs");
                exit(EXIT_FAILURE);
            }
            args->id = vendas_criadas + 1;
            args->id_fake = vendas_criadas + 1;
            args->total_a_vender = total_vendas_por_thread;
            
            pthread_create(&threads[thread_idx], NULL, realizar_venda, (void *)args);
            vendas_criadas++;
            thread_idx++;
        }

        // tenta criar uma thread de CONSULTA se ainda houver alguma para criar
        if (consultas_criadas < num_consultas) {
            ConsultaArgs *args = malloc(sizeof(ConsultaArgs));
            if (!args) {
                perror("Falha ao alocar memória para ConsultaArgs");
                exit(EXIT_FAILURE);
            }
            args->id = consultas_criadas + 1;
            args->id_fake = consultas_criadas + 1;

            pthread_create(&threads[thread_idx], NULL, consultar_estoque, (void *)args);
            consultas_criadas++;
            thread_idx++;
        }
    }

    // aguardando todas as threads terminarem
    for (int i = 0; i < total_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex_venda_consulta);

    printf("\n--- Simulação finalizada ---\n");
    printf("Estoque inicial: %d\n", estoque_inicial);
    printf("Estoque final: %d\n", estoque_compartilhado.quantidade);
    printf("\n");
    printf("Observe se alguma thread de consulta leu o valor errado após uma thread de venda marcar que ele (o valor) seria mudado.\n");
 
    return 0;
}