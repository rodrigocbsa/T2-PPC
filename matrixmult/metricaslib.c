#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include <stdlib.h>

#include <metricaslib.h>

void salvaDados(double tempoDecorridoSerial,int tamanho) {
    FILE *arquivo;

    // Abrir o arquivo para escrita
    arquivo = fopen("tempfile.txt", "w");

    // Escrever o valor double no arquivo
    fprintf(arquivo, "%lf %d", tempoDecorridoSerial, tamanho);

    // Fechar o arquivo
    fclose(arquivo);
}

void calculaMetricas(double tempoDecorrido_Paralelo,int numThreads){
    // Abrir o arquivo para leitura
    FILE *arquivo = fopen("tempfile.txt", "r");

    // Ler o valor double do arquivo
    double tempoDecorrido_Serial;
    int tamanho;
    fscanf(arquivo, "%lf %d", &tempoDecorrido_Serial, &tamanho);

    double speedup, eficiencia;

	speedup = tempoDecorrido_Serial/tempoDecorrido_Paralelo;
	eficiencia = speedup / numThreads;

	printf("\n\nMetricas - %d NLINES e %d threads: \nTempo de Execucao (serial):\t%lf\nTempo de Execucao (paralelo):\t%lf\nSpeedup:\t%lf\nEficiencia:\t%lf\n",tamanho,numThreads,
	tempoDecorrido_Serial,tempoDecorrido_Paralelo,speedup,eficiencia);


    // Fechar o arquivo
    fclose(arquivo);

    // Resetar o ponteiro do arquivo para o início
    arquivo = fopen("tempfile.txt", "r");

    // Fechar o arquivo no final
    fclose(arquivo);
}
