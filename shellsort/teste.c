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

int main() {
    const int size = 100;
    const int numThreads = 2;
    int arr[size];

    // Inicializa o vetor com valores aleatórios
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 100;
    }

    // Mede o tempo de execução serial
    double startSerial = getCurrentTime();
    shellSortSerial(arr, size);
    double endSerial = getCurrentTime();
    double timeSerial = endSerial - startSerial;

    // Reinicializa o vetor para a versão paralela
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 100;
    }

    // Mede o tempo de execução paralela
    double startParallel = getCurrentTime();
    omp_set_num_threads(numThreads);
    shellSortParallel(arr, size);
    double endParallel = getCurrentTime();
    double timeParallel = endParallel - startParallel;

    // Calcula Speedup e Eficiência
    double speedup = timeSerial / timeParallel;
    double efficiency = speedup / numThreads;

    // Exibe os resultados no terminal
    printf("Metricas - Tamanho do Vetor: %d e %d threads:\n", size, numThreads);
    printf("Tempo de Execucao (serial):   %f\n", timeSerial);
    printf("Tempo de Execucao (paralelo): %f\n", timeParallel);
    printf("Speedup:                      %f\n", speedup);
    printf("Eficiencia:                   %f\n", efficiency);

    // Salva os resultados em um arquivo
    FILE *file = fopen("resultados.txt", "w");
    fprintf(file, "Metricas - Tamanho do Vetor: %d e %d threads:\n", size, numThreads);
    fprintf(file, "Tempo de Execucao (serial):   %f\n", timeSerial);
    fprintf(file, "Tempo de Execucao (paralelo): %f\n", timeParallel);
    fprintf(file, "Speedup:                      %f\n", speedup);
    fprintf(file, "Eficiencia:                   %f\n", efficiency);
    fclose(file);

    return 0;
}
