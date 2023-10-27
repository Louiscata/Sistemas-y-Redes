#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

void main(){

    double x = 0.0;
    int i, j, N = 4000, tam = 1000000;
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < tam; j++) {
            x += sqrt((double)j);
        }
    }
    printf(" Resultado = %e\n", x);
}