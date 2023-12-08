#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <libppc.h>
#include <metricaslib.h>
#include <math.h>

#define NLINES 10
#define NCOLS NLINES + 1 // Uma coluna a mais pois é dedicada aos termos independentes

// Descomente esta linha abaixo para imprimir valores das matrizes 
#define __DEBUG__
clock_t inicio, fim;

#define M(i, j, cols, matrix) matrix[(i) * (cols) + (j)]

double *gaussEliminationSerial(double *matrix);
double *gaussEliminationParallel(double *matrix);
double *generate_random_double_matrix2(int rows, int cols);

int main(int argc, char ** argv) {

    srand(time(NULL));

    /**
     * 
     * Matrizes em C precisam ser alocadas como vetores 
     * acessadas pela regra M(i,j) = M[ i * colunas + j ]
     * 
     * */
    double *m1, *mR;

    if (access("m1.dat", F_OK) != 0) {

        printf("\nGenerating new Matrix 1 values...");
        m1 = generate_random_double_matrix2(NLINES, NCOLS);

        save_double_matrix(m1, NLINES, NCOLS, "m1.dat");

    } else {

        printf("\nLoading Matrix 1 from file ...");
        m1 = load_double_matrix("m1.dat", NLINES, NCOLS);

    }

#ifdef __DEBUG__
    print_double_matrix(m1, NLINES, NCOLS);
#endif

    /* 
        Execute o programa da primeira vez na forma serial. 
        Ele ira gerar automaticamente as matrizes m1 e m2, as quais serão salvas e usadas automaticamente 
        nas execucoes futuras
        
    */

    //mR = gaussEliminationSerial(m1);
	mR = gaussEliminationParallel(m1);

#ifdef __DEBUG__
    printf("\nResulting matrix:");
    print_double_matrix(mR, NLINES, NCOLS);
#endif

    /* 
        Quando voce executar o programa da primeira vez com a implementacao serial, 
        ele ira criar o arquivo mR_1.dat.
        
        Nas execucoes seguintes, utilize a versao paralela. 
        Ele entao ira criar o arquivo mR_2.dat e comparar com o anterior a fim de verificar se 
        a implementacao serial e paralela estao dando as mesmas respostas
    */

    if (access("mR_1.dat", F_OK) != 0) {

        /*
        Primeira execucao (serial), salva a resposta
        */
        save_double_matrix(mR, NLINES, NCOLS, "mR_1.dat");

        printf("\nSerial result saved");

    } else {

        /* 
            Segunda e sucessivas execucoes (paralelas): salva a resposta e compara com a anterior
        */
        save_double_matrix(mR, NLINES, NCOLS, "mR_2.dat");

        printf("\nParallel result saved - comparing serial and parallel outputs ...");

        int matrixes_are_equal = compare_double_matrixes_on_files("mR_1.dat",
            "mR_2.dat",
            NLINES,
            NCOLS);

        if (matrixes_are_equal) {
            printf("\nOK! Serial and parallel outputs are equal!");
        } else {
            printf("\nERROR! Files are NOT equal! Something is wrong (probably on parallel version)!");
        }
    }

    printf("\n");

    free(m1);
    free(mR);

    return 0;
}

double *gaussEliminationSerial(double *matrix) {
    /* Tempo de Execução */
    inicio = clock();

    /* ENTRANDO EM REGIÃO CRÍTICA */
    for (int i = 0; i < NLINES; i++) {
        // Pivotização parcial (troca de linhas)
        // maxRow: DEPENDÊNCIA DE DADOS
        int maxRow = i;
        for (int k = i + 1; k < NLINES; k++) {
            if (fabs(M(k, i, NCOLS, matrix)) > fabs(M(maxRow, i, NCOLS, matrix))) {
                maxRow = k;
            }
        }

        // Troca as linhas
        for (int k = i; k <= NLINES; k++) {
            // temp: DEPENDÊNCIA DE DADOS
            double temp = M(i, k, NCOLS, matrix);
            M(i, k, NCOLS, matrix) = M(maxRow, k, NCOLS, matrix);
            M(maxRow, k, NCOLS, matrix) = temp;
        }

        // Eliminação gaussiana
        /* ENTRANDO EM REGIÃO CRÍTICA */
        for (int k = i + 1; k < NLINES; k++) {
            // factor: DEPENDÊNCIA DE DADOS
            double factor = M(k, i, NCOLS, matrix) / M(i, i, NCOLS, matrix);
            for (int j = i; j <= NLINES; j++) {
                M(k, j, NCOLS, matrix) -= factor * M(i, j, NCOLS, matrix);
            }
        }
    }

    // Substituição retroativa
    /* ENTRANDO EM REGIÃO CRÍTICA */
    for (int i = NLINES - 1; i >= 0; i--) {
        M(i, NLINES, NCOLS, matrix) /= M(i, i, NCOLS, matrix);
        /* ENTRANDO EM REGIÃO CRÍTICA */
        for (int k = i - 1; k >= 0; k--) {
            M(k, NLINES, NCOLS, matrix) -= M(k, i, NCOLS, matrix) * M(i, NLINES, NCOLS, matrix);
        }
    }

    /* Tempo de Execução */
    fim = clock();
    long double tempoDecorrido = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    salvaDados(tempoDecorrido, NLINES);

    return matrix;
}

double *gaussEliminationParallel(double *matrix) {
    /* Tempo de Execução */
    inicio = clock();

    int numThreads = 2;
    omp_set_num_threads(numThreads);

    #pragma omp parallel
    {
        int maxRow;

        #pragma omp for
        for (int i = 0; i < NLINES; i++) {
            // Pivotização parcial (troca de linhas)
            maxRow = i;
            for (int k = i + 1; k < NLINES; k++) {
                if (fabs(M(k, i, NCOLS, matrix)) > fabs(M(maxRow, i, NCOLS, matrix))) {
                    maxRow = k;
                }
            }

            // Troca as linhas
            for (int k = i; k < NCOLS; k++) {
                double temp = M(i, k, NCOLS, matrix);
                M(i, k, NCOLS, matrix) = M(maxRow, k, NCOLS, matrix);
                M(maxRow, k, NCOLS, matrix) = temp;
            }

            // Eliminação gaussiana
            double factor;
            for (int k = i + 1; k < NLINES; k++) {
                factor = M(k, i, NCOLS, matrix) / M(i, i, NCOLS, matrix);
                for (int j = i; j < NCOLS; j++) {
                    M(k, j, NCOLS, matrix) -= factor * M(i, j, NCOLS, matrix);
                }
            }
        }

        // Substituição retroativa
        #pragma omp for
        for (int i = NLINES - 1; i >= 0; i--) {
            M(i, NLINES, NCOLS, matrix) /= M(i, i, NCOLS, matrix);
            for (int k = i - 1; k >= 0; k--) {
                M(k, NLINES, NCOLS, matrix) -= M(k, i, NCOLS, matrix) * M(i, NLINES, NCOLS, matrix);
            }
        }
    }

    /* Tempo de Execução */
    fim = clock();
    double tempoDecorrido = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    calculaMetricas(tempoDecorrido, numThreads);

    return matrix;
}

double *generate_random_double_matrix2(int rows, int cols) {
    srand(time(NULL));

    double *matrix = (double *)malloc(rows * cols * sizeof(double));

    // Gere coeficientes aleatórios
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols - 1; j++) {
            M(i, j, cols, matrix) = (rand() % 10) + 1; // Coeficientes aleatórios entre 1 e 10
        }
    }

    // Gere termos independentes de forma que as equações sejam consistentes
    for (int i = 0; i < rows; i++) {
        M(i, cols - 1, cols, matrix) = 0;
        for (int j = 0; j < cols - 1; j++) {
            M(i, cols - 1, cols, matrix) -= M(i, j, cols, matrix); // Ajuste dos termos independentes
        }
    }

    return matrix;
}
