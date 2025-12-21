# Trabalhos Práticos de Sistemas Operacionais I

Este repositório será usado para o desenvolvimento dos Trabalhos Práticos demonstrando conceitos de Sistemas Operacionais.

## Trabalho prático 1

Este trabalho prático tem por principal objetivo a familiarização com os conceitos de escalonamento e concorrência. Foi utilizado sleep() para forçar uma Thread a esperar pela execução de outra, a fim de causar race conditions e demonstrar como aplicações Semáforos resolvem eles. A partir disso foram criados vários cenários para testar isso.

### Estrutura do Trabalho prático 1

```text
/tp1/
|
|--> /Processos Leitores x Escritores/
|        |
|        |-> 1.1-leitores_sem_controle(leitura_suja).c
|        |-> 1.2-ambos_com_controle_concocrrencia.c
|        |-> 1.3-sem_controle_concorrencia.c
|        |--> /executaveis/      <- contem os executaveis dessa versão do trabalho
|
|--> /Processos Produtores x Consumidores/
|        |-> 2.1-versão_com_vários_processos_produtores_e_1_consumidor
|        |-> 2.2-versão_com_vários_processos_produtores_e_vários_consumidores
|        |-> 2.3-sem_controle_concorrencia.c
|
|-> SO_TP1_2025-2.pdf  <- enunciado do trabalho
|-> Relatório.pdf  <- relatório contendo a análise da equipe sobre os resultados
```

## Trabalho Prático 2 

Esse trabalho prático foi feito com o propósito de demonstrar o ganho de desempenho oferecido pela implementação de Threads, tanto ao partir uma tarefa em várias partes menores que podem ser processadas paralelamente quanto em gerar threads que lidam com uma requisição ao invés de congelar o sistema toda vez que uma for feita.

### Estrutura do Trabalho Prático 2

```text
/tp2/
|
|--> /multiplicação_matricial/
|        |-> matriz_tradicional.c
|        |-> matriz_tradicional # versão já compilada
|        |-> matriz_paralela.c
|        |-> matriz_paralela # versão já comiplada
|        |-> gera_grafico.py
|        |-> /data/ # diretório contendo os dados de tempo das 2 máquinas pedidas no enunciado
|        -> /graficos/ # diretório contendo os gráficos gerados a partir dos dados em /data/
|
|--> /produto_escalar/
|        |-> produto_escalar_paralelo.c
|        |-> produto_escalar_paralelo
|        |-> produto_escalar_sequencial.c
|        |-> produto_escalar_sequencial
|        |-> graficos.py
|        |-> /dados_resultados/
|        |-> /graficos/
|
|--> /Simulação de Servidor Web/
|    |-> servidor_paralelo.c
|    |-> servidor_paralelo
|    |-> servidor_sequencial.c
|    |-> servidor_sequencial
|    |-> simulacao_de_funcoes.c # contem as funções que são usadas pelos dois servidoress
|    |-> simulacao_de_funcoes.h
|
|-> Relatório SO tp2.pdf
|-> SO_TP2_2025-2.pdf
```

# Licença

Este repositório foi feito com objetivos académicos, sinta-se livre para utilizá-lo e aprender com ele como desejar.