#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define NUM_HILOS 2

int variable_global = 0;
//lo global es el puntero
int *local_hilo1;

void *funcion_hilo1(void *variable_local)
{

    int *local_main = (int *)variable_local;
    *local_main = 42;
    int variable_hilo1 = 2;
    //asignamos al puntero el valor de la local
    local_hilo1 = &variable_hilo1;

    printf("Aqui el hilo 1. Mi variable local vale %d\n", variable_hilo1);
    printf("Aqui el hilo 1. La variable global es %d\n", variable_global);

    variable_global = 2;

    sleep(1); // Le damos tiempo al hilo 2 para que cambie la variable
    printf("Aqui el hilo 1. Mi variable local ahora es %d\n", variable_hilo1);

    pthread_exit(EXIT_SUCCESS);
}

void *funcion_hilo2(void *variable_local)
{

    int *local2 = (int *)variable_local;

    *local_hilo1 = 3;

    pthread_exit(EXIT_SUCCESS);
}

int main()
{
    pthread_t hilo1, hilo2;
    int variable_local = 33;

    printf("Aquí el main. Mi variable local vale %d\n", variable_local);

    if (pthread_create(&hilo1, NULL, funcion_hilo1, &variable_local) != 0)
    {
        perror("Error al crear el hilo\n");
        return EXIT_FAILURE;
    }

    if (pthread_create(&hilo2, NULL, funcion_hilo2, &variable_local) != 0)
    {
        perror("Error al crear el hilo\n");
        return EXIT_FAILURE;
    }

    pthread_join(hilo1, NULL);

    printf("HILO 1 terminado\n");
    printf("Aquí el main. Mi variable local vale %d\n", variable_local);
    printf("Aquí el main. La variable global vale %d\n", variable_global);

    pthread_join(hilo2, NULL);

    printf("HILO 2 terminado\n");
    printf("Aquí el main. Mi variable local vale %d\n", variable_local);

    return EXIT_SUCCESS;
}