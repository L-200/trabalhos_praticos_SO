// comando para compilar: gcc servidor_sequencial.c simulacao_de_funcoes.c -o servidor_sequencial
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "simulacao_de_funcoes.h" // Inclui as funções de simulação

int main() {
    // Variáveis de medição de tempo
    clock_t inicio, fim;
    double tempo_total;
    
    int num_clientes; // Variável para armazenar a entrada do usuário com relação à quantidade de clientes (requisições) a serem processadas
    int i; 
    
    printf("--- SERVIDOR WEB SIMULADO: VERSAO SEQUENCIAL ---\n");
    printf("OBJETIVO: Demonstracao do bloqueio do sistema (sem threads).\n");

    // Entrada Dinâmica do Usuário
    printf("\n[SERVIDOR]: Digite a quantidade de clientes (requisicoes) a simular: ");
    if (scanf("%d", &num_clientes) != 1 || num_clientes <= 0) {
        printf("[ERRO]: Entrada invalida. Encerrando.\n");
        return 1;
    }
    
    printf("\n[SERVIDOR]: Simulacao iniciada para %d clientes. Cada um sera atendido em serie...\n", num_clientes);
    
    // SIMULAÇÃO DE CARGA SEQUENCIAL COM MEDIÇÃO DE TEMPO
    inicio = clock(); // Marca o início da execução

    // Loop que atende um cliente de cada vez devido ao não-paralelismo
    for (i = 1; i <= num_clientes; i++) {
        printf("Qual tipo de requisição que o novo cliente quer fazer? Digite o número correspondente.\n");
        printf("1. Login (Muito Leve)\n");
        printf("2. HTML (Leve)\n");
        printf("2. Consulta ao Banco de Dados (Média)\n");
        printf("3. Imagem (Pesada)\n");
        printf("5. Vídeo (Muito pesada)\n");
        
        int tipo_requisicao;
        scanf ("%d", &tipo_requisicao);
        
        printf("\n[SERVIDOR]: Cliente %d Chegou. Processando...\n", i);
        switch (tipo_requisicao)
        {
            case 1:
                processar_login(i);
                break;
            case 2:
                processar_html(i);
                break;
            case 3:
                processar_consulta(i);
                break;
            case 4:
                processar_imagem(i);
                break;
            case 5:
                processar_video(i);
                break;
            default:
                printf("[ERRO]: Tipo de requisição inválido. Encerrando.\n");
                return 1;
        }
        

    }

    // FINALIZAÇÃO E CÁLCULO DE TEMPO
    fim = clock(); // Marca o fim da execução
    
    tempo_total = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("\n--- RESULTADO SEQUENCIAL ---\n");
    printf("Total de Requisicoes Atendidas: %d\n", num_clientes);
    printf("Tempo TOTAL de Processamento (Bloqueado): %.2f segundos.\n", tempo_total);
    printf("-------------------------------------------\n");

    return 0;
}
