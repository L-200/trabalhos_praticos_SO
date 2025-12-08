#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "simulação_de_funcoes.h" // Inclui as funções de simulação

int main() {
    // Variáveis de medição de tempo
    clock_t inicio, fim;
    double tempo_total;
    
    int num_clientes; // Variável para armazenar a entrada do usuário com relação à quantidade de clientes (requisições) a serem processadas
    int i; 
    
    printf("--- SERVIDOR WEB SIMULADO: VERSÃO SEQUENCIAL ---\n");
    printf("OBJETIVO: Demonstração do bloqueio do sistema (sem threads).\n");

    // 1. Entrada Dinâmica do Usuário
    printf("\n[SERVIDOR]: Digite a quantidade de clientes (requisições) a simular: ");
    if (scanf("%d", &num_clientes) != 1 || num_clientes <= 0) {
        printf("[ERRO]: Entrada inválida. Encerrando.\n");
        return 1;
    }
    
    printf("\n[SERVIDOR]: Simulação iniciada para %d clientes. Cada um será atendido em série...\n", num_clientes);
    
    // 2. SIMULAÇÃO DE CARGA SEQUENCIAL COM MEDIÇÃO DE TEMPO
    inicio = clock(); // Marca o início da execução

    // Loop que atende um cliente de cada vez (FORTE BLOQUEIO)
    for (i = 1; i <= num_clientes; i++) {
        printf("\n[SERVIDOR]: Cliente %d (de %d) Chegou. Processando...\n", i, num_clientes);
        
        // Aqui, para simplificar a simulação, faremos todos os clientes pedirem a tarefa mais pesada (5s).
        // Na versão interativa final, poderemos adicionar uma escolha aleatória de função.
        
        // Chamada da função pesada que contém o sleep(5)
        processar_imagem_pesada(i); // BLOQUEIA o programa por 5 segundos
    }

    // 3. FINALIZAÇÃO E CÁLCULO DE TEMPO
    fim = clock(); // Marca o fim da execução
    
    tempo_total = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("\n--- RESULTADO SEQUENCIAL ---\n");
    printf("Total de Requisições Atendidas: %d\n", num_clientes);
    // Tempo teórico: num_clientes * 5 segundos
    printf("Tempo TOTAL de Processamento (Bloqueado): %.2f segundos.\n", tempo_total);
    printf("-------------------------------------------\n");

    return 0;
}
