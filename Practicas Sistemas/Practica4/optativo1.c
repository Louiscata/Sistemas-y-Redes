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

#define NUM_HIJOS 4
#define MAX 1000000000

// Función para la fórmula matemática. Se pasa la iteración actual por parámetros

double formula(double i)
{
    double sumpar = 0.;

    sumpar = (1. / pow(16., i)) * ((4. / ((8. * i) + 1.)) -
                                   (2. / ((8. * i) + 4.)) -
                                   (1. / ((8. * i) + 5.)) -
                                   (1. / ((8. * i) + 6.)));

    return sumpar;
}

int main()
{
    struct timespec start, end;
    pid_t hijos[NUM_HIJOS];
    int pipelines[NUM_HIJOS][2];

    // Comenzamos a medir
    clock_gettime(CLOCK_MONOTONIC, &start); // Registra el tiempo de inicio

    // Creación de los hijos
    for (int i = 0; i < NUM_HIJOS; i++)
    {
        // Pipeline para comunicar el resultado al padre
        if (pipe(pipelines[i]) == -1)
        {
            perror("Error al crear la tubería");
            exit(1);
        }

        if ((hijos[i] = fork()) < 0)
        {
            perror("Error al crear los hijos");
            exit(1);
        }
        else if (hijos[i] == 0) // Código de los hijos
        {
            double sumalocal = 0;

            printf("Soy el hijo %d con PID %d. Voy a comenzar los cálculos\n", i, getpid());

            // Suma parcial
            for (int j = i * 10; j < MAX; j += NUM_HIJOS * 10)
            {
                for (int k = j; k < j + 10; k++)
                {
                    sumalocal += formula((double)k);
                }
            }

            // Comunicación del resultado
            write(pipelines[i][1], &sumalocal, sizeof(double));

            printf("Soy el hijo %d con PID %d. He terminado. Mi suma es: %.40lf\n", i, getpid(), sumalocal);

            close(pipelines[i][1]);

            exit(EXIT_SUCCESS);
        }
        else
        {
            close(pipelines[i][1]);
        }
    }

    printf("Aqui el main. He creado los hijos. Voy a esperar a que acaben.\n");

    int estado;

    // El main espera a que los hijos terminen
    for (int i = 0; i < NUM_HIJOS; i++)
    {
        waitpid(hijos[i], &estado, 0);
        if (WIFEXITED(estado))
        {
            int exit_estado = WEXITSTATUS(estado);
            printf("El hijo %d termino con valor de salida: %d\n", i, exit_estado);
        }
    }

    // Resultado con las sumas parciales
    double suma = 0, sumaparcial = 0;
    for (int i = 0; i < NUM_HIJOS; i++)
    {
        read(pipelines[i][0], &sumaparcial, sizeof(double));
        suma += sumaparcial;
        close(pipelines[i][0]);
    }

    // Terminamos de medir
    clock_gettime(CLOCK_MONOTONIC, &end); // Registra el tiempo de finalización

    printf("Aqui el main. La suma total de las sumas parciales de los procesos es %.40lf\n", suma);

    // Calculamos el tiempo total
    double tiempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Se ha tardado %.10lf segundos en calcularse utilizando procesos.\n\n", tiempo);

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