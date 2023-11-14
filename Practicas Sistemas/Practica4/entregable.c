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

long int ITER_THREAD[NUM_HILOS];

double SUM_THREAD[NUM_HILOS];

// Función de la fórmula matemática. Se pasa la iteración actual por parámetros.
double formula(double i)
{
    double sumpar = 0.;

    sumpar = (1. / pow(16., i)) * ((4. / ((8. * i) + 1.)) -
                                   (2. / ((8. * i) + 4.)) -
                                   (1. / ((8. * i) + 5.)) -
                                   (1. / ((8. * i) + 6.)));

    return sumpar;
}

// Función de los hilos
void *thread_funcion(void *i)
{
    int tid = i;
    double sumalocal = 0;

    printf("Soy el hilo %d. Voy a comenzar los cálculos\n", tid);

    // Suma parcial
    for (int j = (tid - 1) * 10; j < MAX; j += NUM_HILOS * 10)
    {
        for (int k = j; k < j + 10; k++)
        {
            // Código para comprobar las iteraciones
            // ITER_THREAD[tid - 1]++;
            // printf("%d:%ld\n", tid, ITER_THREAD[tid - 1]);

            sumalocal += formula((double)k);
        }
    }

    // Guardamos el resultado
    SUM_THREAD[tid - 1] = sumalocal;

    printf("Soy el hilo %d. He terminado. Mi suma es: %.40lf\n", tid, SUM_THREAD[tid - 1]);

    pthread_exit(NULL);
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

    // Comenzamos a medir
    clock_gettime(CLOCK_MONOTONIC, &start); // Registra el tiempo de inicio

    // Creación de los hilos
    for (int i = 1; i <= NUM_HILOS; i++)
    {
        if (pthread_create(&hilos[i - 1], NULL, thread_funcion, (void *)i) != 0)
        {
            perror("Error al crear el hilo\n");
            return EXIT_FAILURE;
        }
    }

    printf("Aqui el main. He creado los hilos. Voy a esperar a que acaben.\n");

    // Finalización de los hilos
    for (int i = 1; i <= NUM_HILOS; i++)
    {
        pthread_join(hilos[i - 1], NULL);
        printf("HILO %d terminado\n", i);
    }

    // Código para comprobar las iteraciones
    /*
    long int iteraciones = 0;
    for(int i = 0; i < NUM_HILOS; i++){
        iteraciones += ITER_THREAD[i];
    }
    printf("Aqui el main. Los hilos han acabado. Esta es la suma de las iteraciones: %ld\n", iteraciones);
    */

    // Suma de los hilos
    double suma = 0;
    for (int i = 0; i < NUM_HILOS; i++)
    {
        suma += SUM_THREAD[i];
    }

    // Terminamos de medir
    clock_gettime(CLOCK_MONOTONIC, &end); // Registra el tiempo de finalización

    printf("Aqui el main. La suma total de las sumas parciales de los hilos es %.40lf\n", suma);

    // Calculamos el tiempo total
    double tiempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Se ha tardado %.10lf segundos en calcularse utilizando hilos.\n\n", tiempo);

    printf("Aqui el main. Voy a comenzar los cálculos de manera secuencial\n");

    // Comenzamos a medir
    clock_gettime(CLOCK_MONOTONIC, &start); // Registra el tiempo de inicio

    // Suma secuencial
    double sumasec = 0;
    for (int i = 0; i < MAX; i++)
    {
        sumasec += formula(i);
    }

    // Terminamos de medir
    clock_gettime(CLOCK_MONOTONIC, &end); // Registra el tiempo de finalización

    printf("Aqui el main. La suma total secuencial es %.40f\n", sumasec);

    // Calculamos el tiempo total
    tiempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Se ha tardado %.10lf segundos en calcularse de manera secuencial.\n\n", tiempo);

    printf("La diferencia de las sumas debido a la pérdida de precisión en punto flotante es %.40lf\n", suma - sumasec);

    return EXIT_SUCCESS;
}