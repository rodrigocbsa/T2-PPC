#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include <libppc.h>

#define SIZE 10
#define MIN_V 1
#define MAX_V 1000

// Descomente esta linha abaixo para imprimir valores dos vetores
#define __DEBUG__




int main(int argc, char ** argv){

	srand( time(NULL) );
    
	int *v1, *v2, *vR;

	if (access("v1.dat", F_OK) != 0) {
	
		printf("\nGenerating new Vector 1 values...");
		v2 = (int*)generate_random_int_vector(SIZE,MIN_V,MAX_V);
		save_int_vector(v2,SIZE,"v1.dat");
					

	} else {
	
		printf("\nLoading Vector 1 from file ...");
		v1 = load_int_vector("v1.dat",SIZE);
		
	}

#ifdef __DEBUG__
	print_int_vector( v1 , SIZE, 3);
#endif

	if (access("v2.dat", F_OK) != 0) {
	
	
		printf("\nGenerating new Vector 2 values...");
		v2 = (int*)generate_random_int_vector(SIZE,MIN_V,MAX_V);
		
		save_int_vector(v2,SIZE,"v2.dat");
					

	} else {

		printf("\nLoading Vector 2 from file ...");
		v2 = load_int_vector("v2.dat",SIZE);

	}

#ifdef __DEBUG__
	print_int_vector(v2,SIZE,3);
#endif

	vR = bucketSortSerial( v1, SIZE );
	vR = bucketSortParalelo( v2, SIZE );

#ifdef __DEBUG__
	printf("\nResulting vector ordenation:");
	print_int_vector(v1,SIZE,3);
#endif	

	if (access("vR_1.dat", F_OK) != 0) {

		/*
		Primeira execucao (serial), salva a resposta
		*/
		save_int_vector(vR,SIZE,"vR_1.dat");
		
		printf("\nSerial result saved");

	} else {
		
		/* 
			Segunda e sucessivas execucoes (paralelas): salva a resposta e compara com a anterior
		*/
		save_int_vector(vR,SIZE, "vR_2.dat");
		
		printf("\nParallel result saved - comparing serial and parallel outputs ...");

		int vectors_are_equal = compare_int_vectors_on_files("vR_1.dat", "vR_2.dat");
		
		if ( vectors_are_equal ){
			printf("\nOK! Serial and parallel outputs are equal!");
		} else {
			printf("\nERROR! Files are NOT equal! Something is wrong (probably on parallel version)!");
		}		
	}
	
	printf("\n");

	free( v1 );
	free( v2 );
	free( vR );

	return 0;
}


void shellSortSerial(int array[], int tamanho) {
    for (int intervalo = tamanho / 2; intervalo > 0; intervalo /= 2) {
        for (int i = intervalo; i < tamanho; i++) {
            int chave = array[i];
            int j = i;

            while (j >= intervalo && array[j - intervalo] > chave) {
                array[j] = array[j - intervalo];
                j -= intervalo;
            }

            array[j] = chave;
        }
    }
}

void shellSortParalelo(int array[], int tamanho) {
    for (int intervalo = tamanho / 2; intervalo > 0; intervalo /= 2) {
        #pragma omp parallel for shared(array, tamanho, intervalo) default(none)
        for (int i = intervalo; i < tamanho; i++) {
            int chave = array[i];
            int j = i;

            while (j >= intervalo && array[j - intervalo] > chave) {
                array[j] = array[j - intervalo];
                j -= intervalo;
            }

            array[j] = chave;
        }
    }
}