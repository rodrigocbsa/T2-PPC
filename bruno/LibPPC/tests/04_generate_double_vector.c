#include <libppc.h>

#include <stdlib.h>

#include <stdio.h>

// TODO: test negative numbers

int main(){

    double *v = (double*)malloc( sizeof(double) * 1000000);

    double *histogram = (double*)malloc( sizeof( double)  * 10 );

    for ( int i = 0; i < 10; i++ ){
        v[ i ] = histogram[ i ] = 0;
    }

    generate_random_double_vector( v, 1000000, 0, 10);

    for (int i = 0; i < 1000000; i++){

        if ( v[ i ] >= 0 && v[i] <= 10 ){

            histogram[ (int)v[i] ] ++;

            continue;

        }
            
        return -1;
    }

    for ( int i = 0; i < 10; i ++){
        printf("%d\n", (int)histogram[ i ]);
    }

    free( v ) ;

    return 0;
}

