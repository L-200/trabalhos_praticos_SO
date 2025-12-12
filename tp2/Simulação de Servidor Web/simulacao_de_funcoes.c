
#include "simulacao_de_funcoes.h"
#include <stdio.h>
#include <unistd.h> 

void processar_login(int req_id) {
   // Para 0.5s, usamos 500.000 microssegundos e usleep()
    long tempo_us = 500000;
    printf("        [REQUISIÇÃO %d]: INICIO - Processando Login (0.5s)...\n", req_id);
    usleep(tempo_us); 
    printf("        [REQUISIÇÃO %d]: FIM - Login concluido.\n", req_id);
}
void processar_html(int req_id) {
    int tempo = 1; // 1 segundo
    printf("[REQUISICAO %d]: INICIO - Processando HTML (Tarefa Leve, %ds)...\n", req_id, tempo);
    sleep(tempo);
    printf("[REQUISICAO %d]: FIM - HTML concluido.\n", req_id);
}

void processar_consulta(int req_id) {
    int tempo = 3; // 3 segundos
    printf("[REQUISICAO %d]: INICIO - Processando Consulta DB (Tarefa Media, %ds)...\n", req_id, tempo);
    sleep(tempo);
    printf("[REQUISICAO %d]: FIM - Consulta DB concluida.\n", req_id);
}

void processar_imagem(int req_id) {
    int tempo = 5; // 5 segundos
    printf("[REQUISICAO %d]: INICIO - Processando Imagem (Tarefa Pesada, %ds)...\n", req_id, tempo);
    sleep(tempo);
    printf("[REQUISICAO %d]: FIM - Imagem concluida.\n", req_id);
}

void processar_video(int req_id){
    int tempo = 8; // 8 segundos
    printf("[REQUISICAO %d]: INICIO - Processando Video (Tarefa Muito Pesada, %ds)...\n", req_id, tempo);
    sleep(tempo);
    printf("[REQUISICAO %d]: FIM - Video concluido.\n", req_id);
}

