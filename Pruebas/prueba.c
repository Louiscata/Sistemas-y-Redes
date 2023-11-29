#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main () {
    double *arreglo;
    char dummy;

    printf("PID: %d\n", getpid());

    // Reservar memoria inicialmente
    arreglo = (double *)malloc(1000 * sizeof(double));

    if (arreglo == NULL) {
        fprintf(stderr, "Error al reservar memoria.\n");
        return 1;
    }

    printf("sizeof() del puntero: %ld", sizeof(arreglo));

    scanf(" %c", &dummy);

    arreglo++;

    printf("sizeof() del puntero: %ld", sizeof(arreglo));

    scanf(" %c", &dummy);

    // Liberar la memoria al final
    free(arreglo);


    return EXIT_SUCCESS;
}