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


###