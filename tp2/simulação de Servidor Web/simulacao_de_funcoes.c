
#include "simulation_functions.h"
#include <stdio.h>
#include <unistd.h> 

void processar_html(int req_id) {
    int tempo = 1; // 1 segundo
    printf("[REQUISIÇÃO %d]: INICIO - Processando HTML (Tarefa Leve, %ds)...\n", req_id, tempo);
    sleep(tempo);
    printf("[REQUISIÇÃO %d]: FIM - HTML concluído.\n", req_id);
}

void processar_consulta(int req_id) {
    int tempo = 3; // 3 segundos
    printf("[REQUISIÇÃO %d]: INICIO - Processando Consulta DB (Tarefa Média, %ds)...\n", req_id, tempo);
    sleep(tempo);
    printf("[REQUISIÇÃO %d]: FIM - Consulta DB concluída.\n", req_id);
}

void processar_imagem(int req_id) {
    int tempo = 5; // 5 segundos
    printf("[REQUISIÇÃO %d]: INICIO - Processando Imagem (Tarefa Pesada, %ds)...\n", req_id, tempo);
    sleep(tempo);
    printf("[REQUISIÇÃO %d]: FIM - Imagem concluída.\n", req_id);
}
