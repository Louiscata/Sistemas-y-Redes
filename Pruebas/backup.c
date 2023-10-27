#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>

#define N 9  // Valor maximo de x
#define M 20 // Total de numeros con los que va a calcular la media

static void gestion(int); /*Declaracion de la funcion de gestion de señales recibidas*/

void main()
{
    pid_t hijo1, hijo2;
    int pipeline[2];
    FILE *archivo;
    archivo = fopen("archivo.txt", "w+");
    if (archivo == NULL)
    {
        perror("Error al crear el archivo");
        exit(EXIT_FAILURE);
    }

    printf("Aqui el proceso padre con pid %d, voy a crear el pipe\n", getpid());

    // Creacion del pipeline
    if (pipe(pipeline) == -1)
    {
        perror("Ha ocurrido un problema en la creacion del pipeline\n");
        exit(EXIT_FAILURE);
    }
    printf("Aqui el proceso padre con pid %d, voy a crear los hijos\n", getpid());

    // Hijo 1
    if ((hijo1 = fork()) == -1)
    {
        printf("Error en la ejecución del fork\n");
        exit(EXIT_FAILURE);
    }
    else if (hijo1 == 0)
    { /*Trabajo del hijo 1*/
        double media = 0;
        sigset_t set;
        int sig;

        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);

        close(pipeline[0]);
        sleep(1); // para que el proceso 2 tenga tiempo de escribir su pid
        read(pipeline[1], &hijo2, sizeof(pid_t));
        close(pipeline[1]);

        for (int x = 1; x < N; x++)
        {
            srand48(x);
            for (int i = 0; i < M; i++)
            {
                media += drand48();
            }
            media /= M;
            fprintf(archivo, "%.8lf\n", media);
            kill(hijo2, SIGUSR1);
            sigwait(&set, &sig);
        }
        exit(0);
    }

    // Hijo 2
    if ((hijo2 = fork()) == -1)
    {
        printf("Error en la ejecución del fork\n");
        exit(EXIT_FAILURE);
    }
    else if (hijo2 == 0)
    {
        sigset_t set;
        int sig;
        double media, sumatorio, valor, dt;

        // Va a enviar su pid al hijo 1
        hijo2 = getpid();
        close(pipeline[0]);
        write(pipeline[1], &hijo2, sizeof(pid_t));
        close(pipeline[1]);

        // Solo va a continuar si le llega SIGUSR2
        sigemptyset(&set);
        sigaddset(&set, SIGUSR2);

        // Leer
        for (int x = 1; x < N; x++)
        {
            sigwait(&set, &sig);

            sumatorio=0;
            fscanf(archivo, "%lf\n", &media);
            kill(hijo1, SIGUSR1);

            srand48(x);
            for (int i = 0; i < M; i++)
            {
                valor = drand48();
                sumatorio += (valor - media) * (valor - media);
            }
            
            sumatorio/=M;
            dt=sqrt(sumatorio);

            printf("Aquí el hijo 2 con pid %d. He calculado la desvia ion tipicapara x = %d. El resultado es %lf", getppid(), x, dt);
        }
        exit(0);
    }

    hijo1 = wait(NULL);
    printf("Aqui el proceso padre con pid %d, he esperado por el hijo con pid %d\n", getpid(), hijo1);
    hijo2 = wait(NULL);
    printf("Aqui el proceso padre con pid %d, he esperado por el hijo con pid %d\n", getpid(), hijo2);

    close(pipeline[0]);
    close(pipeline[1]);
}

static void gestion(int numero_de_senhal)
{ /* Funcion de gestion de señales*/
    switch (numero_de_senhal)
    {
    case SIGUSR1:
        printf("Señal SIGUSR1 recibida. Soy %d\n", getpid());
        break;
    case SIGUSR2:
        printf("Señal SIGUSR2 recibida. Soy %d\n", getpid());
        break;
    }

    return;
}
