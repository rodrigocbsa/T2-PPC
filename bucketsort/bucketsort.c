#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <omp.h>

#include <libppc.h>

#define SIZE 400
#define MIN_V 10
#define MAX_V 100

int numThreads = 4;

// Descomente esta linha abaixo para imprimir valores dos vetores
//#define __DEBUG__


int *bucketSortParalelo(int v[], int tamanho);
int *bucketSortSerial(int v[], int tamanho);

int main(int argc, char ** argv){

    omp_set_num_threads(numThreads);

	srand( time(NULL) );
    
	int *v1, *vR;

	if (access("v1.dat", F_OK) != 0) {
	
		printf("\nGenerating new Vector 1 values...");
		v1 = (int*)generate_random_int_vector(SIZE,MIN_V,MAX_V);

		save_int_vector(v1,SIZE,"v1.dat");
					

	} else {
	
		printf("\nLoading Vector 1 from file ...");
		v1 = load_int_vector("v1.dat",SIZE);
		
	}

#ifdef __DEBUG__
	print_int_vector( v1 , SIZE, 1);
#endif

	//vR = bucketSortSerial( v1, SIZE );
	vR = bucketSortParalelo( v1, SIZE );

#ifdef __DEBUG__
	printf("\nResulting vector ordenation:");
	print_int_vector(vR,SIZE, 1);
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
	free( vR );

	return 0;
}


int *bucketSortSerial(int arr[], int n) {


    // declare an array of buckets
    int *bucket = (int *)malloc(sizeof(int) * n);

    // calculate the number of buckets needed
    int numBuckets = (int)sqrt(n);

    // parallelize the placement of each element into its corresponding bucket
    for (int i = 0; i < n; i++) {
        //int bucketIndex = arr[i] / numBuckets;
        bucket[i] = arr[i];
    }

    // parallelize the sorting of each bucket using a separate sorting algorithm
    for (int i = 0; i < numBuckets; i++) {
        // use selection sort to sort each bucket
        for (int j = i * n / numBuckets; j < (i + 1) * n / numBuckets; j++) {
            int minIndex = j;
            for (int k = j + 1; k < n; k++) {
                if (bucket[k] < bucket[minIndex])
                    minIndex = k;
            }

            // swap the elements
            int temp = bucket[j];
            bucket[j] = bucket[minIndex];
            bucket[minIndex] = temp;
        }
    }

    // parallelize the combining of the sorted buckets into a single, sorted array
    for (int i = 0; i < numBuckets; i++) {
        for (int j = i * n / numBuckets; j < (i + 1) * n / numBuckets; j++) {
            arr[j] = bucket[i * n / numBuckets + j % (n / numBuckets)];
        }
    }

    free(bucket);

    return arr;
}

// Function to sort an array using bucket sort
int *bucketSortParalelo(int arr[], int n) {


    // declare an array of buckets
    int *bucket = (int *)malloc(sizeof(int) * n);

    // calculate the number of buckets needed
    int numBuckets = (int)sqrt(n);

    // parallelize the placement of each element into its corresponding bucket
    #pragma omp parallel for shared(arr, bucket, n, numBuckets) default(none)
    for (int i = 0; i < n; i++) {
        //int bucketIndex = arr[i] / numBuckets;
        bucket[i] = arr[i];
    }

    // parallelize the sorting of each bucket using a separate sorting algorithm
    #pragma omp parallel for shared(bucket, n, numBuckets) default(none)
    for (int i = 0; i < numBuckets; i++) {
        // use selection sort to sort each bucket
        for (int j = i * n / numBuckets; j < (i + 1) * n / numBuckets; j++) {
            int minIndex = j;
            for (int k = j + 1; k < n; k++) {
                if (bucket[k] < bucket[minIndex])
                    minIndex = k;
            }

            // swap the elements
            int temp = bucket[j];
            bucket[j] = bucket[minIndex];
            bucket[minIndex] = temp;
        }
    }

    // parallelize the combining of the sorted buckets into a single, sorted array
    #pragma omp parallel for shared(arr, bucket, n, numBuckets) default(none)
    for (int i = 0; i < numBuckets; i++) {
        for (int j = i * n / numBuckets; j < (i + 1) * n / numBuckets; j++) {
            arr[j] = bucket[i * n / numBuckets + j % (n / numBuckets)];
        }
    }

    free(bucket);

    return arr;
}