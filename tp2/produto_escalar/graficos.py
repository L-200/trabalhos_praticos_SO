import pandas as pd
import matplotlib.pyplot as plt


# importar os arquivos e titularizar
colunas_sequencial = ["tamanho_vet", "resultado_PI", "tempo(ms)"]
colunas_paralelo = ["qtd_threads", "tamanho_vet", "resultado_PI", "tempo(ms)"]
df_paralelo = pd.read_csv("dados_resultados/dados_paralelo.csv", header=None, names=colunas_paralelo)
df_sequencial = pd.read_csv("dados_resultados/dados_sequencial.csv", header=None, names=colunas_sequencial)

# Tirar linha de titulo colacada pelo script em C
df_paralelo = df_paralelo[1:]
df_sequencial = df_sequencial[1:]

# plotar gráfico para cada quantidade de thread
qtd_threads = df_paralelo["qtd_threads"]
tempos_sequencial = pd.to_numeric(df_sequencial["tempo(ms)"])

for qtd_thread in qtd_threads.unique():

    #separar dados
    df_thread = df_paralelo[df_paralelo["qtd_threads"] == qtd_thread]
    tempos_paralelo  = pd.to_numeric(df_thread["tempo(ms)"])
    tamanho_vet = pd.to_numeric(df_thread["tamanho_vet"])
    
    plt.figure()  
    plt.plot(tamanho_vet, tempos_paralelo, marker='o')
    plt.plot(tamanho_vet, tempos_sequencial, marker='o')
    plt.xlabel("TAMANHO DO VETOR")
    plt.ylabel("TEMPO (ms)")
    plt.title("RESULTADOS DOS CÁLCULOS DO PRODUTO INTERNO")
    plt.grid(True)

    plt.savefig(f"./graficos/RESULTADOS_COM_{qtd_thread}_THREADS.png")
    plt.show()
    plt.close()

