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
#define MAX 100000

long int ITER_THREAD1 = 0;
long int ITER_THREAD2 = 0;
long int ITER_THREAD3 = 0;

double SUM_THREAD1=0;
double SUM_THREAD2=0;
double SUM_THREAD3=0;

void *thread_funcion(void *i)
{
    switch ((int)i)
    {
    case (1):
        printf("Soy el hilo 1. Voy a comenzar los cálculos.\n");

        for (int j = 0; j < MAX; j += 30){
            for (int k = 0; k < j + 10; k++) {
                ITER_THREAD1++;
                printf("1:%ld\n", ITER_THREAD1);
            }
        }

        printf("Soy el hilo 1. He terminado.\n");

        pthread_exit(EXIT_SUCCESS);

        break;
    case (2):
        printf("Soy el hilo 2. Voy a comenzar los cálculos.\n");

        for (int j = 10; j < MAX; j += 30){
            for (int k = 0; k < j + 10; k++) {
                ITER_THREAD2++;
                printf("2:%ld\n", ITER_THREAD2);
            }
        }

        printf("Soy el hilo 2. Voy a terminar.\n");

        pthread_exit(EXIT_SUCCESS);

        break;
    case (3):
        printf("Soy el hilo 3. Voy a comenzar los cálculos.\n");

        for (int j = 20; j < MAX; j += 30){
            for (int k = 0; k < j + 10; k++) {
                ITER_THREAD3++;
                printf("3:%ld\n", ITER_THREAD3);
            }
        }

        printf("Soy el hilo 3. Voy a terminar.\n");

        pthread_exit(EXIT_SUCCESS);

        break;

        break;
    }
}

//float sumatorio

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

    printf("Aqui el main. Los hilos han acabado. Esta es la suma de las iteraciones: %ld\n", ITER_THREAD1 + ITER_THREAD2 + ITER_THREAD3);
    printf("Aqui el main. Voy a comenzar los cálculos de manera secuencial\n");

    return EXIT_SUCCESS;
}