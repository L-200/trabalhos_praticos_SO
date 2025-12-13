#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int id;
    int inicio;
    int fim;
    double* vetorA;
    double* vetorB;
    double somaParcial;
} ThreadData;



// ----- VARIAVEIS GLOBAIS -------
double* vetorA;
double* vetorB;
int tamanhos_vet[10] = {100, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500};

int QTD_TAMANHO_VET;
int QTD_THREADS;
int TAMANHO_VETOR;





// --- ALGORITMO SEQUENCIAL ---
double produtoEscalarSequencial(int tamanho) {
    double soma  = 0.0;
    for (int i = 0; i< tamanho; i++){
        soma += vetorA[i] * vetorB[i];
    }
    return soma;
}


// --- PreencherVetores com valores fixos ---
void preencherVetores(int tamanho){
    vetorA = (double*) malloc(tamanho * sizeof(double));
    vetorB = (double*) malloc(tamanho * sizeof(double));

    for (int i = 0; i < tamanho; i++){
        vetorA[i] = 1.5;
        vetorB[i] = 2.0;
    }
}


int main() {
    // --- Declarações de variaveis locais da main ---
    struct timespec start, end;
    FILE* arquivo;

    
    // --- CONFIGURAÇÕES INICIAIS DOS ARTEFATOS GERADOS ---
    arquivo = fopen("./dados_resultados/dados_sequencial.csv", "w");
    if( arquivo == NULL){
        printf("Erro ao abrir o arquivo");
        return 0;
    }
    fprintf(arquivo, "tamanho_vet, resultado_PI, tempo(ms)\n");
    


    printf("=========================== PRODUTO ESCALAR SEQUENCIAL (S/Pthreads) ===========================\n\n");


    // --- Entrada do usuário ---
    printf("Digite a quantidade de tamanhos diferentes de vetor para comparação(ex: 2 resultará testes para: [100, 200]): ");
    scanf("%d", &QTD_TAMANHO_VET);
    if(QTD_TAMANHO_VET>10){
        printf("Quantidade acima do esperado.\n");
        return 0;
    }

    // --- CÁLCULO PARA DIFERENTES VERSÕES ---
    for( int tamanho_vet = 0 ; tamanho_vet < QTD_TAMANHO_VET; tamanho_vet++){

        
        // --- DADOS INICIAIS NECESSÁRIOS ---
        TAMANHO_VETOR = tamanhos_vet[tamanho_vet];
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TAM VET %d ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", TAMANHO_VETOR);
        printf("Alocando memoria e gerando vetores de tamanho %d\n\n", TAMANHO_VETOR);
        preencherVetores(TAMANHO_VETOR);

        
        // --- TESTE SEQUENCIAL ---
        printf("Executando Sequencial, Com %d elementos para cada vetor\n", TAMANHO_VETOR);
        
        
        // --- CÁLCULO DO PRODUTO INTERNO ---
        clock_gettime(CLOCK_MONOTONIC, &start);
        srand(time(NULL));
        double resPar = produtoEscalarSequencial(TAMANHO_VETOR);
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        
        // --- Analise e registro dos resultados ---
        double tempo_total = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Resultado do Produto Escalar: %.2f | Tempo: %.4f ms\n\n", resPar, tempo_total);
        fprintf(arquivo, "%d, %.2f, %.4f\n", TAMANHO_VETOR, resPar, tempo_total);
        
        // --- Limpar memória ---
        free(vetorA);
        free(vetorB);

    }    
    

    printf("===========================  FIM DO PRODUTO ESCALAR SEQUENCIAL (S/Pthreads) ===========================\n\n");
    fclose(arquivo);

    return 0;
}
