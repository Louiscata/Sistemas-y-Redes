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
#define MAX 100

void *thread_funcion(void *i)
{
    switch ((int)i)
    {
    case (1):
        printf("Soy el hilo 1. Hoy me siento generoso.\n");

        for (int j = 0; j < MAX * 2; j++)
            sched_yield();

        printf("Soy el hilo 1. Voy a terminar.\n");

        pthread_exit(EXIT_SUCCESS);

        break;
    case (2):
        printf("Soy el hilo 2. Yo también estoy generoso, pero menos.\n");

        for (int j = 0; j < MAX; j++)
            sched_yield();

        printf("Soy el hilo 2. Voy a terminar.\n");

        pthread_exit(EXIT_SUCCESS);

        break;
    case (3):

        printf("Soy el hilo 3. Dadme vuestras cpus.\n");

        pthread_exit(EXIT_SUCCESS);

        break;
    }
}

int main()
{
    pthread_t hilos[NUM_HILOS];

    for (int i = 1; i <= NUM_HILOS; i++)
    {
        if (pthread_create(&hilos[i], NULL, thread_funcion, (void *)i) != 0)
        {
            perror("Error al crear el hilo\n");
            return EXIT_FAILURE;
        }
    }

    printf("Aqui el main. He creado los hilos. Voy a esperar a que acaben.\n");


    for (int i = 1; i <= NUM_HILOS; i++)
    {
        pthread_join(hilos[i], NULL);
        printf("HILO %d terminado\n", i);
    }

    printf("Aqui el main. Los hilos han acabado. Voy a terminar\n");

    return EXIT_SUCCESS;
}