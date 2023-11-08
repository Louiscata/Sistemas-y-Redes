#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define NUM_HILOS 3
#define MAX 1000000000
#define STACK_SIZE 65536

long int ITER_THREAD[NUM_HILOS];

double SUM_THREAD[NUM_HILOS];

// se pasa la iteración actual por parámetros

double formula(double i)
{
    double sumpar = 0.;

    sumpar = (1. / pow(16., i)) * ((4. / ((8. * i) + 1.)) -
                                   (2. / ((8. * i) + 4.)) -
                                   (1. / ((8. * i) + 5.)) -
                                   (1. / ((8. * i) + 6.)));

    return sumpar;
}

int thread_funcion(void *i)
{
    int tid = *(int *)i;
    double sumalocal = 0;

    printf("Soy el hilo %d. Voy a comenzar los cálculos\n", tid);

    for (int j = (tid - 1) * 10; j < MAX; j += NUM_HILOS * 10)
    {
        for (int k = j; k < j + 10; k++)
        {
            sumalocal += formula((double)k);
        }
    }

    SUM_THREAD[tid - 1] = sumalocal;

    printf("Soy el hilo %d. He terminado. Mi suma es: %.40lf\n", tid, SUM_THREAD[tid - 1]);

    return (EXIT_SUCCESS);
}

int main()
{
    struct timespec start, end;

    for (int i = 0; i < NUM_HILOS; i++)
    {
        ITER_THREAD[i] = 0;
        SUM_THREAD[i] = 0;
    }

    pthread_t hilos[NUM_HILOS];
    char *stacks[NUM_HILOS];
    for (int i = 0; i < NUM_HILOS; i++)
    {
        stacks[i] = (char *)malloc(STACK_SIZE);
    }

    // Comenzamos a medir
    // clock_t start = clock();
    clock_gettime(CLOCK_MONOTONIC, &start); // Registra el tiempo de inicio

    for (int i = 1; i <= NUM_HILOS; i++)
    {
        int *tid = (int *)malloc(sizeof(int));
        *tid = i;

        // Crear hilos utilizando clone()
        hilos[i - 1] = clone(thread_funcion, stacks[i - 1] + STACK_SIZE, CLONE_VM, tid);
        if (hilos[i - 1] == -1)
        {
            perror("Error al crear el hilo");
            return EXIT_FAILURE;
        }
    }

    printf("Aqui el main. He creado los hilos. Voy a esperar a que acaben.\n");
    sleep(12);
    int estado;

    for (int i = 0; i < NUM_HILOS; i++)
    {
        waitpid(hilos[i], &estado, 0);
        if (WIFEXITED(estado))
        {
            int exit_estado = WEXITSTATUS(estado);
            printf("El hilo %d terminó con valor de salida: %d\n", i, exit_estado);
        }
    }

    double suma = 0;
    for (int i = 0; i < NUM_HILOS; i++)
    {
        suma += SUM_THREAD[i];
    }
    // Terminamos de medir
    // clock_t end = clock();
    clock_gettime(CLOCK_MONOTONIC, &end); // Registra el tiempo de finalización

    printf("Aqui el main. La suma total de las sumas parciales de los hilos es %.40lf\n", suma);

    // Calculamos el tiempo que pasa
    // double tiempo = (double)(end - start) / CLOCKS_PER_SEC;
    double tiempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Se ha tardado %.10lf segundos en calcularse utilizando hilos.\n\n", tiempo);

    printf("Aqui el main. Voy a comenzar los cálculos de manera secuencial\n");
    // Comenzamos a medir
    // start = clock();
    clock_gettime(CLOCK_MONOTONIC, &start); // Registra el tiempo de inicio

    double sumasec = 0;
    for (int i = 0; i < MAX; i++)
    {
        sumasec += formula(i);
    }
    // Terminamos de medir
    // end = clock();
    clock_gettime(CLOCK_MONOTONIC, &end); // Registra el tiempo de finalización

    printf("Aqui el main. La suma total secuencial es %.40f\n", sumasec);
    // tiempo = (double)(end - start) / CLOCKS_PER_SEC;
    tiempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Se ha tardado %.10lf segundos en calcularse de manera secuencial.\n\n", tiempo);

    printf("La diferencia de las sumas debido a la pérdida de precisión en punto flotante es %.40lf\n", suma - sumasec);

    return EXIT_SUCCESS;
}
// gcc -o optativo2 optativo2.c -lm -lpthread