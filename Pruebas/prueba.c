#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

static void gestion(int); /*Declaracion de la funcion de gestion de señales recibidas*/

void main()
{
    pid_t hijo1, hijo2, pid_esperado;
    struct sigaction sig_usr1, sig_usr2, sig_int;
    int status, codigo_error;

    sig_usr1.sa_flags = SA_RESTART;
    sig_usr1.sa_handler = gestion;
    sig_usr1.sa_flags = 0;
    codigo_error=sigaction(SIGUSR1, &sig_usr1, NULL);
    if(codigo_error==-1){
        perror("Error al definir el gestor de SIGUSR1");
        exit(-1);
    }

    sig_usr2.sa_flags = SA_RESTART;
    sig_usr2.sa_handler = gestion;
    sig_usr2.sa_flags = 0;
    codigo_error=sigaction(SIGUSR2, &sig_usr2, NULL);
    if(codigo_error==-1){
        perror("Error al definir el gestor de SIGUSR2");
        exit(-1);
    }

    sig_int.sa_flags = SA_RESTART;
    sig_int.sa_handler = gestion;
    sig_int.sa_flags = 0;
    codigo_error=sigaction(SIGINT, &sig_int, NULL);
    if(codigo_error==-1){
        perror("Error al definir el gestor de SIGINT");
        exit(-1);
    }

    if ((hijo1 = fork()) == -1)
    {
        printf("Error en la ejecución del fork\n");
        exit(0);
    }
    else if (hijo1 == 0)
    {
        printf("Aqui el hijo 1 con pid %d. Voy a enviar la senhal SIGUSR1 al padre\n", getpid());
        kill(getppid(), SIGUSR1);
        sleep(5);
        kill(getppid(), SIGUSR2);
        sleep(2);
        exit(0);
    }

    printf("Aqui el padre con pid %d, voy a crear al hijo 2\n", getpid());
    if ((hijo2 = fork()) == -1)
    {
        printf("Error en la ejecución del fork\n");
        exit(0);
    }
    else if (hijo2 == 0)
    {
        printf("Aqui el hijo 2 con pid %d\n, voy a dormir y a mandar SIGINT", getpid());
        sleep(1);
        kill(getppid(), SIGINT);
        exit(0);
    }
    
    printf("Aqui el padre con pid %d\n. Voy a esperar a que acabe el hijo 1", getpid());
    pid_esperado = waitpid(hijo2, &status, 0);
    printf("Soy el proceso padre con pid %d y he esperado a que acabe el proceso con pid %d\n", getpid(), pid_esperado);
    if ((WIFEXITED(status)))
    {
        printf("Soy el padre, he esperado por ej hijo2 que ha acabado con status %d\n", WEXITSTATUS(status));
    }
    else
    {
        printf("El hijo 2 ha terminado de forma inesperada\n");
    }

    pid_esperado = waitpid(hijo1, &status, 0);
    printf("Soy el proceso padre con pid %d y he esperado a que acabe el proceso con pid %d\n", getpid(), pid_esperado);
    if ((WIFEXITED(status)))
    {
        printf("Soy el padre, he esperado por ej hijo1 que ha acabado con status %d\n", WEXITSTATUS(status));
    }
    else
    {
        printf("El hijo 1 ha terminado de forma inesperada\n");
    }
}

static void gestion(int numero_de_senhal)
{ /* Funcion de gestion de señales*/
    switch (numero_de_senhal)
    {
    case SIGUSR1: /*Entra señal SIGUSR1*/
        printf("Señal SIGUSR1 recibida. Soy %d\n", getpid());
        break;
    /*Completa para el resto de las señales usadas*/
    case SIGUSR2:
        printf("Señal SIGUSR2 recibida. Soy %d\n", getpid());
        break;
   /*  case SIGINT: 
        break; */
    }
    
    return;
}

