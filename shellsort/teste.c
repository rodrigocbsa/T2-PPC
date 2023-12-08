#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void shellSortSerial(int arr[], int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
                arr[j] = arr[j - gap];
            arr[j] = temp;
        }
    }
}

void shellSortParallel(int arr[], int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        #pragma omp parallel for
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
                arr[j] = arr[j - gap];
            arr[j] = temp;
        }
    }
}

double getCurrentTime() {
    return omp_get_wtime();
}

void runAndMeasure(void (*sortFunction)(int[], int), const char *label, int arr[], int size, int numThreads) {
    double start = getCurrentTime();
    sortFunction(arr, size);
    double end = getCurrentTime();
    double time = end - start;

    printf("Tempo de Execucao (%s): %f\n", label, time);

    // Salva os resultados em um arquivo específico para cada versão
    FILE *file = fopen(label, "w");
    fprintf(file, "%f", time);
    fclose(file);
}

int main() {
    const int size = 100;
    const int numThreads = 2;
    int arr[size];

    // Inicializa o vetor com valores aleatórios
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 100;
    }

    // Executa e mede o tempo da versão serial
    runAndMeasure(shellSortSerial, "serial_time.txt", arr, size, numThreads);

    // Reinicializa o vetor para a versão paralela
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 100;
    }

    // Executa e mede o tempo da versão paralela
    runAndMeasure(shellSortParallel, "parallel_time.txt", arr, size, numThreads);

    // Calcula Speedup e Eficiência usando os tempos salvos
    FILE *serialFile = fopen("serial_time.txt", "r");
    FILE *parallelFile = fopen("parallel_time.txt", "r");

    double serialTime, parallelTime;
    fscanf(serialFile, "%lf", &serialTime);
    fscanf(parallelFile, "%lf", &parallelTime);

    double speedup = serialTime / parallelTime;
    double efficiency = speedup / numThreads;

    // Exibe os resultados no terminal
    printf("Metricas - Tamanho do Vetor: %d e %d threads:\n", size, numThreads);
    printf("Tempo de Execucao (serial):   %f\n", serialTime);
    printf("Tempo de Execucao (paralelo): %f\n", parallelTime);
    printf("Speedup:                      %f\n", speedup);
    printf("Eficiencia:                   %f\n", efficiency);

    // Salva os resultados em um arquivo
    FILE *metricsFile = fopen("metrics.txt", "w");
    fprintf(metricsFile, "Metricas - Tamanho do Vetor: %d e %d threads:\n", size, numThreads);
    fprintf(metricsFile, "Tempo de Execucao (serial):   %f\n", serialTime);
    fprintf(metricsFile, "Tempo de Execucao (paralelo): %f\n", parallelTime);
    fprintf(metricsFile, "Speedup:                      %f\n", speedup);
    fprintf(metricsFile, "Eficiencia:                   %f\n", efficiency);
    fclose(metricsFile);

    // Fechar os arquivos utilizados
    fclose(serialFile);
    fclose(parallelFile);

    return 0;
}
