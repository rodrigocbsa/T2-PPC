int main() {
    int array[] = {38, 27, 43, 3, 9, 82, 10};
    int tamanho = sizeof(array) / sizeof(array[0]);

    // Define o número de threads para OpenMP
    omp_set_num_threads(4);



/*
SEÇÃO DE DESCRIÇÃO DO ALGORITMO



*/


#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include <libppc.h>

#define NLINES 3
#define NCOLS 3

// Descomente esta linha abaixo para imprimir valores das matrizes 
#define __DEBUG__


void bucketSortParalelo(int array[], int tamanho);
void bucketSortSerial(int array[], int tamanho);
void ordenaBucket(int bucket[], int tamanho);


int main(int argc, char ** argv){

	srand( time(NULL) );
    
	double *m1, *m2, *mR;

	if (access("m1.dat", F_OK) != 0) {
	
		printf("\nGenerating new Matrix 1 values...");
		m1 = (double*)generate_random_double_matrix( NLINES, NCOLS );
		
		save_double_matrix( m1, NLINES, NCOLS, "m1.dat");
					

	} else {
	
		printf("\nLoading Matrix 1 from file ...");
		m1 = load_double_matrix("m1.dat", NLINES, NCOLS);
		
	}

#ifdef __DEBUG__
	print_double_matrix( m1 , 3, 3);
#endif

	if (access("m2.dat", F_OK) != 0) {
	
	
		printf("\nGenerating new Matrix 2 values...");
		m2 = (double*)generate_random_double_matrix( NLINES, NCOLS );
		
		save_double_matrix( m2, NLINES, NCOLS, "m2.dat");
					

	} else {

		printf("\nLoading Matrix 2 from file ...");
		m2 = load_double_matrix("m2.dat", NLINES, NCOLS);

	}

#ifdef __DEBUG__
	print_double_matrix( m2 , 3, 3);
#endif
	
	/* 
		Execute o programa da primeira vez na forma serial. 
		Ele ira gerar automaticamente as matrizes m1 e m2, as quais serão salvas e usadas automaticamente 
		nas execucoes futuras
		
		
	*/
	
	mR = bucketSortSerial( m1, m2 );		
	mR = bucketSortParalelo( m1, m2 );

#ifdef __DEBUG__
	printf("\nResulting matrix multiplication:");
	print_double_matrix( mR , 3, 3);
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
		save_double_matrix( mR, NLINES, NCOLS, "mR_1.dat");
		
		printf("\nSerial result saved");


	} else {
		
		/* 
			Segunda e sucessivas execucoes (paralelas): salva a resposta e compara com a anterior
		*/
		save_double_matrix( mR, NLINES, NCOLS, "mR_2.dat");
		
		printf("\nParallel result saved - comparing serial and parallel outputs ...");

		int matrixes_are_equal = compare_double_matrixes_on_files("mR_1.dat", 
			"mR_2.dat",
			NLINES,
			NCOLS);
		
		if ( matrixes_are_equal ){
			printf("\nOK! Serial and parallel outputs are equal!");
		} else {
			printf("\nERROR! Files are NOT equal! Something is wrong (probably on parallel version)!");
		}		
	}
	
	printf("\n");

	free( m1 );
	free( m2 );
	free( mR );

	return 0;
}


void ordenaBucket(int bucket[], int tamanho) {
    for (int i = 1; i < tamanho; i++) {
        int chave = bucket[i];
        int j = i - 1;

        while (j >= 0 && bucket[j] > chave) {
            bucket[j + 1] = bucket[j];
            j = j - 1;
        }
        bucket[j + 1] = chave;
    }
}

void bucketSortSerial(int array[], int tamanho) {
    const int numBuckets = 10;
    int buckets[numBuckets][tamanho];
    int tamanhos[numBuckets];

    for (int i = 0; i < numBuckets; i++) {
        tamanhos[i] = 0;
    }

    for (int i = 0; i < tamanho; i++) {
        int indice = numBuckets * array[i] / 100;
        buckets[indice][tamanhos[indice]++] = array[i];
    }

    for (int i = 0; i < numBuckets; i++) {
        ordenaBucket(buckets[i], tamanhos[i]);
    }

    int indiceAtual = 0;
    for (int i = 0; i < numBuckets; i++) {
        for (int j = 0; j < tamanhos[i]; j++) {
            array[indiceAtual++] = buckets[i][j];
        }
    }
}

void bucketSortParalelo(int array[], int tamanho) {
    const int numBuckets = 10;
    int buckets[numBuckets][tamanho];
    int tamanhos[numBuckets];

    for (int i = 0; i < numBuckets; i++) {
        tamanhos[i] = 0;
    }

    #pragma omp parallel for shared(array, buckets, tamanhos, numBuckets) default(none)
    for (int i = 0; i < tamanho; i++) {
        int indice = numBuckets * array[i] / 100;
        int thread_id = omp_get_thread_num();
        buckets[indice][tamanhos[indice]++] = array[i];
    }

    #pragma omp parallel for shared(buckets, tamanhos, numBuckets) default(none)
    for (int i = 0; i < numBuckets; i++) {
        ordenaBucket(buckets[i], tamanhos[i]);
    }

    int indiceAtual = 0;
    #pragma omp parallel for shared(array, buckets, tamanhos, numBuckets) default(none)
    for (int i = 0; i < numBuckets; i++) {
        for (int j = 0; j < tamanhos[i]; j++) {
            array[indiceAtual++] = buckets[i][j];
        }
    }
}