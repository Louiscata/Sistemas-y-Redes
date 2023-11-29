#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    printf("Aquí el ejercicio 1, mi pid es %d\n", getpid());

    if (argc != 2)
    {
        perror("Número de argumentos incorrecto");
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1)
    {
        perror("Error al obtener la información del archivo");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Contenido del archivo usando read:\n");
    char buffer;
    while (read(fd, &buffer, 1) > 0)
    {
        putchar(buffer);
    }

    printf("Esperando... justo antes de mmap()\n");
    getchar();
    
    void *fileMemory = mmap(NULL, fileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (fileMemory == MAP_FAILED)
    {
        perror("Error al proyectar el archivo en memoria");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Esperando... después de mmap()\n");
    getchar();

    close(fd);

    printf("\n----------------------------------------------\n");

    printf("Contenido de la proyección en memoria:\n");
    for (int i = 0; i < fileStat.st_size; ++i)
    {
        putchar(((char *)fileMemory)[i]);
    }

    if (munmap(fileMemory, fileStat.st_size) == -1)
    {
        perror("Error al cerrar la proyección en memoria");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}