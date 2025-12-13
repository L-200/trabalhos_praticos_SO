#ifndef BENCHMARKING_H
#define BENCHMARKING_H

// Funções auxiliares de matriz
int** cria_matriz(int ordem);
void preenche_matriz(int** matriz, int ordem);
void free_matriz(int** matriz, int ordem);

// Interfaces dos benchmarks
float chama_tradicional(void);
float chama_paralela(void);

// Função para salvar métricas em arquivos
void salvar_metricas(const char* nome_arquivo, int ordem, int threads, double tempo);

#endif
