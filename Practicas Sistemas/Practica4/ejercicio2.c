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

void *funcion_hilo1()
{
    printf("Soy el hilo 1. Todavia no he terminado.\n");

    sleep(10);

    pthread_exit(EXIT_SUCCESS);
}

void *funcion_hilo2()
{
    printf("Soy el hilo 2. Todavia no he terminado.\n");

    sleep(10);

    pthread_exit(EXIT_SUCCESS);
}

void *funcion_hilo3(void *variable_local)
{
    pid_t hijo;
    printf("Soy el hilo 3. Voy a ejecutar el fork().\n");

    if ((hijo = fork()) < 0) // Creacion hijo
    {
        perror("Error al crear el proceso hijo");
        exit(EXIT_FAILURE);
    }

    if (hijo == 0)
    {
        printf("Hola soy el proceso hijo con pid %d y voy a petar tu ordenador\n", getpid());
        // el comando necesario para comprobar el numero de hilos del proceso hijo es ps -T -p PID_del_hijo (o del padre si quiere)
        //RESPUESTA: SE CREA CON UN SOLO HILO
    }
    sleep(20);

    pthread_exit(EXIT_SUCCESS);
}

int main()
{

    pthread_t hilo1, hilo2, hilo3;

    printf("Aqui el main con pid %d. Voy a crear los hilos.\n", getpid());

    if (pthread_create(&hilo1, NULL, funcion_hilo1, NULL) != 0)
    {
        perror("Error al crear el hilo\n");
        return EXIT_FAILURE;
    }

    if (pthread_create(&hilo2, NULL, funcion_hilo2, NULL) != 0)
    {
        perror("Error al crear el hilo\n");
        return EXIT_FAILURE;
    }

    if (pthread_create(&hilo3, NULL, funcion_hilo3, NULL) != 0)
    {
        perror("Error al crear el hilo\n");
        return EXIT_FAILURE;
    }

    printf("Aqui el main. He creado los hilos. Voy a esperar a que acaben.\n");

    pthread_join(hilo1, NULL);
    printf("HILO 1 terminado\n");
    pthread_join(hilo2, NULL);
    printf("HILO 2 terminado\n");
    pthread_join(hilo3, NULL);
    printf("HILO 3 terminado\n");

    return EXIT_SUCCESS;
}