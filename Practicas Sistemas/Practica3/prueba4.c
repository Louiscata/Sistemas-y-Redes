#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

static void gestion(int); /*Declaracion de la funcion de gestion de señales recibidas*/
char *hora();             /*Devuelve la hora*/

void main()
{
    pid_t hijo1, hijo2, pid_esperado;
    struct sigaction sig_usr1, sig_usr2, sig_int;
    int status, codigo_error;

    sig_usr1.sa_flags = SA_RESTART;
    sig_usr1.sa_handler = gestion;
    sig_usr1.sa_flags = 0;
    codigo_error = sigaction(SIGUSR1, &sig_usr1, NULL);
    if (codigo_error == -1)
    {
        perror("Error al definir el gestor de SIGUSR1");
        exit(-1);
    }

    sig_usr2.sa_flags = SA_RESTART;
    sig_usr2.sa_handler = gestion;
    sig_usr2.sa_flags = 0;
    codigo_error = sigaction(SIGUSR2, &sig_usr2, NULL);
    if (codigo_error == -1)
    {
        perror("Error al definir el gestor de SIGUSR2");
        exit(-1);
    }

    sig_int.sa_flags = SA_RESTART;
    sig_int.sa_handler = SIG_IGN;
    sig_int.sa_flags = 0;
    codigo_error = sigaction(SIGINT, &sig_int, NULL);
    if (codigo_error == -1)
    {
        perror("Error al definir el gestor de SIGINT");
        exit(-1);
    }

    // Bloqueamos SIGUSR1
    sigset_t bloqueadas, pendientes;
    printf("[Hora: %s] Aqui el padre con pid %d, voy a bloquear SIGUSR1\n", hora(), getpid());
    sigemptyset(&bloqueadas);
    sigaddset(&bloqueadas, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &bloqueadas, NULL) == -1)
    {
        perror("Error al bloquear la señal SIGUSR1\n");
    }

    // Creamos el hijo 1
    printf("[Hora: %s] Aqui el padre con pid %d, voy a crear al hijo 1\n", hora(), getpid());
    if ((hijo1 = fork()) == -1)
    {
        printf("[Hora: %s] Error en la ejecución del fork\n", hora());
        exit(0);
    }
    else if (hijo1 == 0)
    {
        printf("[Hora: %s] Aqui el hijo 1 con pid %d. Voy a enviar la senhal SIGUSR1 al padre\n", hora(), getpid());
        kill(getppid(), SIGUSR1);
        sleep(5);
        printf("[Hora: %s] Aqui el hijo 1 con pid %d. Voy a enviar la senhal SIGUSR2 al padre\n", hora(), getpid());
        kill(getppid(), SIGUSR2);
        sleep(3);
        printf("[Hora: %s] Aqui el hijo 1 con pid %d. He enviado la senhal y voy a terminar\n", hora(), getpid());

        exit(5);
    }

    // Creamos el hijo 2
    printf("[Hora: %s] Aqui el padre con pid %d, voy a crear al hijo 2\n", hora(), getpid());
    if ((hijo2 = fork()) == -1)
    {
        printf("[Hora: %s] Error en la ejecución del fork\n", hora());
        exit(0);
    }
    else if (hijo2 == 0)
    {
        printf("[Hora: %s] Aqui el hijo 2 con pid %d, voy a mandar SIGINT\n", hora(), getpid());
        sleep(2);
        kill(getppid(), SIGINT);
        exit(0);
    }

    // Comprobamos que SIGUSR1 esta bloqueada
    if (sigpending(&pendientes) == -1)
    {
        perror("Error al comprobar las señales pendientes");
    }

    if (sigismember(&pendientes, SIGUSR1))
    {
        printf("[Hora: %s] Aqui el padre con pid %d. La senhal SIGUSR1 esta bloqueada\n", hora(), getpid());
    }
    else
    {
        printf("[Hora: %s] Aqui el padre con pid %d. La senhal SIGUSR1 no esta bloqueada\n", hora(), getpid());
    }

    // Esperamos a que lleguen SIGUSR1 y SIGUSR2
    sleep(10);

    // Desbloqueamos SIGUSR1
    printf("[Hora: %s] Aqui el padre con pid %d. Voy a desbloquear la senhal SIGUSR1\n", hora(), getpid());
    sigprocmask(SIG_UNBLOCK, &bloqueadas, NULL);

    // Comprobamos que ya no esta bloqueada
    sigemptyset(&pendientes);

    if (sigpending(&pendientes) == -1)
    {
        perror("Error al comprobar las señales pendientes");
    }

    if (sigismember(&pendientes, SIGUSR1))
    {
        printf("[Hora: %s] Aqui el padre con pid %d. La senhal SIGUSR1 esta bloqueada\n", hora(), getpid());
    }
    else
    {
        printf("[Hora: %s] Aqui el padre con pid %d. La senhal SIGUSR1 no esta bloqueada\n", hora(), getpid());
    }

    printf("[Hora: %s] Aqui el padre con pid %d. Voy a esperar a que acabe el hijo 1\n", hora(), getpid());
    pid_esperado = waitpid(hijo1, &status, 0);
    printf("[Hora: %s] Soy el proceso padre con pid %d y he esperado a que acabe el proceso con pid %d\n", hora(), getpid(), pid_esperado);
    if ((WIFEXITED(status)))
    {
        printf("[Hora: %s] Soy el padre, he esperado por el hijo1 que ha acabado con status %d\n", hora(), WEXITSTATUS(status));
    }
    else
    {
        printf("[Hora: %s] El hijo 1 ha terminado de forma inesperada\n", hora());
    }

    printf("[Hora: %s] Aqui el padre con pid %d. Voy a esperar a que acabe el hijo 2\n", hora(), getpid());
    pid_esperado = waitpid(hijo2, &status, 0);
    printf("[Hora: %s] Soy el proceso padre con pid %d y he esperado a que acabe el proceso con pid %d\n", hora(), getpid(), pid_esperado);
    if ((WIFEXITED(status)))
    {
        printf("[Hora: %s] Soy el padre, he esperado por el hijo2 que ha acabado con status %d\n", hora(), WEXITSTATUS(status));
    }
    else
    {
        printf("[Hora: %s] El hijo 2 ha terminado de forma inesperada\n", hora());
    }
}

static void gestion(int numero_de_senhal)
{ /* Funcion de gestion de señales*/
    switch (numero_de_senhal)
    {
    case SIGUSR1: /*Entra señal SIGUSR1*/
        printf("[Hora: %s] Señal SIGUSR1 recibida. Soy %d\n", hora(), getpid());
        break;
    /*Completa para el resto de las señales usadas*/
    case SIGUSR2:
        printf("[Hora: %s] Señal SIGUSR2 recibida. Soy %d\n", hora(), getpid());
        break;
    }

    return;
}

char *hora()
{
    time_t rawtime;
    struct tm *timeinfo;
    static char buffer[9];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%H:%M:%S]", timeinfo);

    return (buffer);
}
