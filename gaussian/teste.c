#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#define NLINES 10
#define NCOLS NLINES + 1

#define M(i, j, cols, matrix) matrix[(i) * (cols) + (j)]

clock_t inicio, fim;

void *print_double_matrix(double *matrix, long int lines, long int columns);

void calculaMetricas(double tempo, int numThreads) {
    // Implemente a lógica para calcular métricas
    // (tempo, numThreads, etc.) se necessário.
    printf("Tempo: %.6f segundos\n", tempo);
    printf("Número de Threads: %d\n", numThreads);
}

double *gaussEliminationParallel(double *matrix, int numLines, int numCols) {
    /* Tempo de Execução */
    inicio = clock();

    int numThreads = 2;
    omp_set_num_threads(numThreads);

    #pragma omp parallel
    {
        int maxRow;

        #pragma omp for
        for (int i = 0; i < numLines; i++) {
            // Pivotização parcial (troca de linhas)
            maxRow = i;
            for (int k = i + 1; k < numLines; k++) {
                if (fabs(M(k, i, numCols, matrix)) > fabs(M(maxRow, i, numCols, matrix))) {
                    maxRow = k;
                }
            }

            // Troca as linhas
            for (int k = i; k < numCols; k++) {
                double temp = M(i, k, numCols, matrix);
                M(i, k, numCols, matrix) = M(maxRow, k, numCols, matrix);
                M(maxRow, k, numCols, matrix) = temp;
            }

            // Eliminação gaussiana
            double factor;
            for (int k = i + 1; k < numLines; k++) {
                factor = M(k, i, numCols, matrix) / M(i, i, numCols, matrix);
                for (int j = i; j < numCols; j++) {
                    M(k, j, numCols, matrix) -= factor * M(i, j, numCols, matrix);
                }
            }
        }

        // Substituição retroativa
        #pragma omp for
        for (int i = numLines - 1; i >= 0; i--) {
            M(i, numLines, numCols, matrix) /= M(i, i, numCols, matrix);
            for (int k = i - 1; k >= 0; k--) {
                M(k, numLines, numCols, matrix) -= M(k, i, numCols, matrix) * M(i, numLines, numCols, matrix);
            }
        }
    }

    /* Tempo de Execução */
    fim = clock();
    double tempoDecorrido = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    calculaMetricas(tempoDecorrido, numThreads);

    return matrix;
}

int main() {
    srand(time(NULL));
    double *mr;
    double *m1 = (double *)malloc(NLINES * NCOLS * sizeof(double));
    
    // Preencha m1 com seus valores ou use sua lógica existente
    // ...

    // Execute a versão paralela
    mr = gaussEliminationParallel(m1, NLINES, NCOLS);

    print_double_matrix(mr, NLINES, NCOLS);
    // Exiba ou faça algo com a matriz resultante
    // ...

    free(m1);

    return 0;
}

void *print_double_matrix(double *matrix, long int lines, long int columns)
{
	long int i, j;

	fprintf(stdout, "\n");

	for ( i = 0; i < lines; i++ ){

		for ( j = 0; j < columns; j++ ){

			fprintf(stdout, "%.3lf", matrix[ i*columns + j ]  );

			if ( j < columns -1 ){
				fprintf(stdout, "\t");
			} else {
				fprintf(stdout, "\n");
			}

		}

		
	}
}