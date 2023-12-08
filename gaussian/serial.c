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

#define NLINES 20
#define NCOLS NLINES + 1 // Uma coluna a mais pois é dedicada aos termos independentes

// Descomente esta linha abaixo para imprimir valores das matrizes 
//#define __DEBUG__
clock_t inicio, fim;

double *gaussEliminationSerial(double *matrix);
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

    mR = gaussEliminationSerial(m1);
	

#ifdef __DEBUG__
    printf("\nResulting matrix:");
    print_double_matrix(mR, NLINES, NCOLS);
#endif



        /*
        Primeira execucao (serial), salva a resposta
        */
        save_double_matrix(mR, NLINES, NCOLS, "mR_1.dat");

        printf("\nSerial result saved");

    

    printf("\n");

    free(m1);
    free(mR);

    return 0;
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