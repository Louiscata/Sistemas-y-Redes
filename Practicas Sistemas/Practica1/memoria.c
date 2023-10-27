#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

int main(int argc, const char *argv[]){

    int *p;
    int i, j, tam = 100000000, N = 20;

    for (i = 0; i < N; i++){
        p = (int*) calloc(tam, sizeof(int));
        for (j = 0; j < tam; j += 369){
            p[j] = 1;
        }
        sleep(1);
    }

    return EXIT_SUCCESS;
}