#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define NUM_HILOS 3

void *thread_funcion(void *i)
{
    switch ((int)i)
    {
    case (1):
        printf("Soy el hilo 1. Todavia no he terminado.\n");

        sleep(20);

        printf("Soy el hilo 1. Voy a terminar.\n");

        pthread_exit(EXIT_SUCCESS);

        break;
    case (2):
        printf("Soy el hilo 2. Todavia no he terminado.\n");

        sleep(20);

        printf("Shou el hilo 2. Voy a terminar.\n");

        pthread_exit(EXIT_SUCCESS);

        break;
    case (3):

        printf("Soy el hilo 3. Voy a ejecutar el exit.\n");

        sleep(3);

        exit(EXIT_SUCCESS);

        //hacer un exit en un hilo es como hacerlo en el main, termina todo.

        break;
    }
}

int main()
{
    pthread_t hilos[NUM_HILOS];

    for (int i = 1; i <= NUM_HILOS; i++)
    {
        if (pthread_create(&hilos[i], NULL, thread_funcion, (void*)i) != 0)
        {
            perror("Error al crear el hilo\n");
            return EXIT_FAILURE;
        }
    }

    printf("Aqui el main. He creado los hilos. Voy a esperar a que acaben.\n");

    sleep(20);

    for (int i = 1; i <= NUM_HILOS; i++)
    {
        pthread_join(hilos[i], NULL);
        printf("HILO %d terminado\n", i);
    }

    return EXIT_SUCCESS;
}