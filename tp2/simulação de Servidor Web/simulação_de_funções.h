#ifndef SIMULADOR_DE_FUNCOES_H
#define SIMULADOR_DE_FUNCOES_H

// Funções para simular diferentes tempos de processamento (I/O simulado com sleep)
// O argumento 'req_id' é usado apenas para identificação nos prints de log.

// Simula uma requisição leve (ex: entrega de um arquivo CSS simples).
void processar_html(int req_id);

// Simula uma requisição de tempo médio (ex: consulta simples ao banco de dados).
void processar_consulta(int req_id);

// Simula uma requisição pesada (ex: download de uma imagem de alta resolução).
void processar_imagem(int req_id);

#endif // SIMULADOR_DE_FUNCOES_H
