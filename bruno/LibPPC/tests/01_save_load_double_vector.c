#include <libppc.h>

#include <stdlib.h>

#include <stdio.h>

int main(){

    double v1[] = { 1.0, 1.1, 1.2 };

    save_double_vector( v1, 3, "teste01.dat" );

    
    double *v2 = (double *)malloc( sizeof(double) * 3 );

    load_double_vector( v2, 3, "teste01.dat" );

    
    for ( int i = 0; i < 3 ; i ++ ){

        if ( v1[ i ] != v2[ i ] ) {
            return -i;
        }
    }

    free( v2 );
    

    return 0;
}

