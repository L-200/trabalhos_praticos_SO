import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

def gerar_graficos_separados(n_cpus):
    # Caminho base (ajustado para sua estrutura de pastas local)
    base_path = 'data'
    
    file_trad = os.path.join(base_path, f'metricas_tradicional_{n_cpus}cpus.csv')
    file_par = os.path.join(base_path, f'metricas_paralela_{n_cpus}cpus.csv')

    # Verificação de arquivos
    if not os.path.exists(file_trad) or not os.path.exists(file_par):
        print(f"Erro: Arquivos não encontrados em '{base_path}'")
        return

    # Carregar dados
    try:
        df_trad = pd.read_csv(file_trad)
        df_par = pd.read_csv(file_par)
    except Exception as e:
        print(f"Erro ao ler CSVs: {e}")
        return

    # Definir escala comum para o eixo Y
    # Isso garante que a comparação visual seja justa, mesmo em arquivos separados
    max_tempo = max(df_trad['Tempo(s)'].max(), df_par['Tempo(s)'].max())
    ylim = (0, max_tempo * 1.1)  # Adiciona 10% de margem no topo

    # Configuração de estilo
    sns.set_theme(style="whitegrid")

    # --- Gráfico 1: Tradicional ---
    plt.figure(figsize=(8, 6))
    sns.lineplot(data=df_trad, x='Ordem', y='Tempo(s)', marker='o', color='blue', label='1 Thread')
    
    plt.title(f'Desempenho: Versão Sequencial ({n_cpus} CPUs)')
    plt.xlabel('Tamanho da Matriz (Ordem)')
    plt.ylabel('Tempo (s)')
    plt.ylim(ylim)  # Aplica a mesma escala
    plt.grid(True)
    plt.legend()
    
    nome_arq_trad = f'grafico_sequencional_{n_cpus}cpus.png'
    plt.savefig(nome_arq_trad)
    plt.close() # Fecha a figura para não sobrepor com a próxima
    print(f"Salvo: {nome_arq_trad}")

    # --- Gráfico 2: Paralelo ---
    plt.figure(figsize=(8, 6))
    sns.lineplot(data=df_par, x='Ordem', y='Tempo(s)', hue='Threads', marker='o', palette='viridis')
    
    plt.title(f'Desempenho: Versão Paralela ({n_cpus} CPUs)')
    plt.xlabel('Tamanho da Matriz (Ordem)')
    plt.ylabel('Tempo (s)')
    plt.ylim(ylim)  # Aplica a mesma escala
    plt.grid(True)
    
    nome_arq_par = f'grafico_paralela_{n_cpus}cpus.png'
    plt.savefig(nome_arq_par)
    plt.close()
    print(f"Salvo: {nome_arq_par}")

if __name__ == "__main__":
    try:
        # Pede ao usuário para digitar o número
        entrada = input("Digite a quantidade de CPUs para gerar os gráficos: ")
        
        # Converte o texto digitado para número inteiro
        qtd_cpus = int(entrada)
        
        # Chama a função
        print(f"Processando dados para {qtd_cpus} CPUs...")
        gerar_graficos_separados(qtd_cpus)
        
    except ValueError:
        print("Erro: Por favor, digite um número inteiro válido.")