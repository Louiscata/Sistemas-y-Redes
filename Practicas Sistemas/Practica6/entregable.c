#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

static void handler(int);
size_t calcular_tamanho_salida(int archivo_entrada, int desplazamiento, int max);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <archivo_entrada> <archivo_salida> \n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR1, handler) == SIG_ERR)
    {
        perror("Error al configurar el manejador de señales para SIGUSR1");
        exit(EXIT_FAILURE);
    }

    int entrada = open(argv[1], O_RDONLY, S_IRUSR | S_IWUSR);
    if (entrada == -1)
    {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }

    struct stat entradaStats;
    if (fstat(entrada, &entradaStats) == -1)
    {
        perror("Error al obtener la información del archivo");
        close(entrada);
        exit(EXIT_FAILURE);
    }

    void *memoria_entrada = mmap(NULL, entradaStats.st_size, PROT_READ, MAP_SHARED, entrada, 0);
    if (memoria_entrada == MAP_FAILED)
    {
        perror("Error al proyectar el archivo de entrada en memoria");
        close(entrada);
        exit(EXIT_FAILURE);
    }

    pid_t hijo = fork();

    if (hijo == -1)
    {
        perror("Error al crear el proceso hijo");
        munmap(memoria_entrada, entradaStats.st_size);
        close(entrada);
        exit(EXIT_FAILURE);
    }

    if (!hijo)
    {
        // Primera mitad del archivo

        sigset_t signal_set;
        sigemptyset(&signal_set);
        sigaddset(&signal_set, SIGUSR1);

        if (sigprocmask(SIG_BLOCK, &signal_set, NULL) == -1)
        {
            perror("Aquí el hijo. Error al bloquear la señal");
            exit(EXIT_FAILURE);
        }

        int senhal_recibida;
        if (sigwait(&signal_set, &senhal_recibida) == -1)
        {
            perror("Aquí el hijo. Error al esperar la señal");
            exit(EXIT_FAILURE);
        }

        printf("Aquí el hijo. Voy a empezar la primera mitad\n");

        int salida = open(argv[2], O_RDWR, S_IRUSR | S_IWUSR);
        if (salida == -1)
        {
            perror("Aquí el hijo. Error al abrir el archivo de salida");
            exit(EXIT_FAILURE);
        }

        struct stat salidaStats;
        if (fstat(salida, &salidaStats) == -1)
        {
            perror("Error al obtener la información del archivo");
            close(salida);
            exit(EXIT_FAILURE);
        }

        void *memoria_salida = mmap(NULL, salidaStats.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, salida, 0);
        if (memoria_salida == MAP_FAILED)
        {
            perror("Aquí el hijo. Error al proyectar el archivo de salida en memoria");
            close(salida);
            exit(EXIT_FAILURE);
        }

        char *buffer;
        buffer = (char *)malloc(salidaStats.st_size * sizeof(char) + 1);

        for (int i = 0; i < salidaStats.st_size; i++)
        {
            if (isdigit(((char *)memoria_salida)[i]))
            {
                buffer[i] = '*';
            }
            else
            {
                buffer[i] = ((char *)memoria_salida)[i];
            }
        }
        buffer[salidaStats.st_size * sizeof(char)] = '\0';

        memcpy(memoria_salida, buffer, salidaStats.st_size);

        free(buffer);

        if (munmap(memoria_salida, salidaStats.st_size) == -1)
        {
            perror("Aquí el hijo. Error al cerrar la proyección en memoria");
            close(entrada);
            close(salida);
            exit(EXIT_FAILURE);
        }

        close(salida);

        ssize_t tamanho_primera_mitad = salidaStats.st_size;

        // Segunda mitad del archivo.

        if (sigprocmask(SIG_BLOCK, &signal_set, NULL) == -1)
        {
            perror("Aquí el hijo. Error al bloquear la señal");
            exit(EXIT_FAILURE);
        }

        if (sigwait(&signal_set, &senhal_recibida) == -1)
        {
            perror("Aquí el hijo. Error al esperar la señal");
            exit(EXIT_FAILURE);
        }

        printf("Aquí el hijo. Voy a comenzar la segunda mitad\n");

        salida = open(argv[2], O_RDWR, S_IRUSR | S_IWUSR);
        if (salida == -1)
        {
            perror("Aquí el hijo. Error al abrir el archivo de salida");
            exit(EXIT_FAILURE);
        }

        if (fstat(salida, &salidaStats) == -1)
        {
            perror("Error al obtener la información del archivo");
            close(salida);
            exit(EXIT_FAILURE);
        }

        memoria_salida = mmap(NULL, salidaStats.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, salida, 0);
        if (memoria_salida == MAP_FAILED)
        {
            perror("Aquí el hijo. Error al proyectar el archivo de salida en memoria");
            close(salida);
            exit(EXIT_FAILURE);
        }

        buffer = (char *)malloc(salidaStats.st_size * sizeof(char));

        for (int i = 0; i < salidaStats.st_size; i++)
        {
            if (isdigit(((char *)memoria_salida)[i]))
            {
                buffer[i] = '*';
            }
            else
            {
                buffer[i] = ((char *)memoria_salida)[i];
            }
        }

        memcpy(memoria_salida, buffer, salidaStats.st_size);

        free(buffer);

        if (munmap(memoria_salida, salidaStats.st_size) == -1)
        {
            perror("Aquí el hijo. Error al cerrar la proyección en memoria");
            close(entrada);
            close(salida);
            exit(EXIT_FAILURE);
        }

        close(salida);

        exit(EXIT_SUCCESS);
    }

    // Primera mitad del archivo

    printf("Aquí el padre. Voy a comenzar la primera mitad\n");

    int salida = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (salida == -1)
    {
        perror("Aquí el padre. Error al abrir el archivo de salida");
        exit(EXIT_FAILURE);
    }

    struct stat salidaStats;
    salidaStats.st_size = calcular_tamanho_salida(entrada, 0, entradaStats.st_size / 2);
    printf("Tamanho de la primera mitad del archivo: %ld\n", salidaStats.st_size);

    char *buffer;
    buffer = (char *)malloc(salidaStats.st_size * sizeof(char));

    int offset_buffer = 0, i;
    for (int i = 0; i < entradaStats.st_size / 2; i++)
    {
        if ((isdigit(((char *)memoria_entrada)[i])))
        {
            int j;
            for (j = 0; j < ((char *)memoria_entrada)[i] - '0'; j++)
            {
                buffer[i + offset_buffer + j] = ((char *)memoria_entrada)[i];
            }
            offset_buffer += j - 1;
        }
        else if ((islower(((char *)memoria_entrada)[i])))
        {
            buffer[i + offset_buffer] = ((char *)memoria_entrada)[i] + 'A' - 'a';
        }
        else
        {
            buffer[i + offset_buffer] = ((char *)memoria_entrada)[i];
        }
    }

    if (truncate(argv[2], salidaStats.st_size) == -1)
    {
        perror("Error al truncar el archivo");
        return 1;
    }

    void *memoria_salida = mmap(NULL, salidaStats.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, salida, 0);
    if (memoria_salida == MAP_FAILED)
    {
        perror("Aquí el padre. Error al proyectar el archivo de salida en memoria");
        close(salida);
        exit(EXIT_FAILURE);
    }

    memcpy(memoria_salida, buffer, salidaStats.st_size);

    free(buffer);

    if (munmap(memoria_salida, salidaStats.st_size) == -1)
    {
        perror("Aquí el padre. Error al cerrar la proyección en memoria");
        close(entrada);
        close(salida);
        exit(EXIT_FAILURE);
    }

    close(salida);

    usleep(500);

    kill(hijo, SIGUSR1);

    // Segunda mitad del archivo

    printf("Aquí el padre. Voy a comenzar la segunda mitad\n");

    salida = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (salida == -1)
    {
        perror("Aquí el padre. Error al abrir el archivo de salida");
        exit(EXIT_FAILURE);
    }

    offset_buffer = salidaStats.st_size - entradaStats.st_size / 2;
    salidaStats.st_size += calcular_tamanho_salida(entrada, entradaStats.st_size / 2, entradaStats.st_size);
    printf("Tamanho total del archivo: %ld\n", salidaStats.st_size);
    buffer = (char *)malloc(salidaStats.st_size * sizeof(char));

    for (i = 0; i < salidaStats.st_size / 2; i++)
    {
        read(salida, buffer + i, salidaStats.st_size / 2);
    }

    for (int i = entradaStats.st_size / 2; i < entradaStats.st_size; i++)
    {
        if ((isdigit(((char *)memoria_entrada)[i])))
        {
            int j;
            for (j = 0; j < ((char *)memoria_entrada)[i] - '0'; j++)
            {
                buffer[i + offset_buffer + j] = ((char *)memoria_entrada)[i];
            }
            offset_buffer += j - 1;
        }
        else if ((islower(((char *)memoria_entrada)[i])))
        {
            buffer[i + offset_buffer] = ((char *)memoria_entrada)[i] + 'A' - 'a';
        }
        else
        {
            buffer[i + offset_buffer] = ((char *)memoria_entrada)[i];
        }
    }

    if (truncate(argv[2], salidaStats.st_size) == -1)
    {
        perror("Error al truncar el archivo");
        return 1;
    }

    memoria_salida = mmap(NULL, salidaStats.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, salida, 0);
    if (memoria_salida == MAP_FAILED)
    {
        perror("Aquí el padre. Error al proyectar el archivo de salida en memoria");
        close(salida);
        exit(EXIT_FAILURE);
    }

    memcpy(memoria_salida, buffer, salidaStats.st_size);

    free(buffer);

    if (munmap(memoria_salida, salidaStats.st_size) == -1)
    {
        perror("Aquí el padre. Error al cerrar la proyección en memoria");
        close(entrada);
        close(salida);
        exit(EXIT_FAILURE);
    }

    close(salida);

    usleep(500);

    kill(hijo, SIGUSR1);

    int status;
    pid_t hijo_terminado = wait(&status);
    if (hijo_terminado == -1)
    {
        perror("wait");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

static void handler(int sid)
{
    switch (sid)
    {
    case SIGUSR1:
        printf("Aquí el proceso con PID [%d]. Senhal SIGUSR1 recibida\n", getpid());
        break;
    }
}

size_t calcular_tamanho_salida(int archivo_entrada, int desplazamiento, int max)
{
    size_t tamanho = 0;
    int caracter = '0', i = desplazamiento;

    if (lseek(archivo_entrada, (off_t)desplazamiento, SEEK_SET) == -1)
    {
        perror("Error al mover el puntero de archivo");
        close(archivo_entrada);
        exit(EXIT_FAILURE);
    }

    while (read(archivo_entrada, &caracter, 1) && i < max)
    {
        if (isdigit(caracter))
        {
            tamanho += (caracter - '0');
        }
        else
        {
            tamanho++;
        }
        i++;
    }

    if (lseek(archivo_entrada, desplazamiento, SEEK_SET) == -1)
    {
        perror("Error al mover el puntero de archivo");
        close(archivo_entrada);
        exit(EXIT_FAILURE);
    }

    return tamanho;
}