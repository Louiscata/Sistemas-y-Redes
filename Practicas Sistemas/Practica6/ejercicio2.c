#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    printf("Aquí el ejercicio 2, mi pid es %d\n", getpid());

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <nombre_del_archivo>\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];

    // Abrir el archivo para lectura y escritura
    int fd = open(filename, O_RDWR);
    if (fd == -1)
    {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Obtener la longitud del archivo con fstat
    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1)
    {
        perror("Error al obtener la información del archivo");
        close(fd);
        exit(EXIT_FAILURE);
    }

    //printf("Esperando... justo antes de mmap()\n");

    //getchar();

    // Proyectar el archivo en memoria con mmap
    void *fileMemory = mmap(NULL, fileStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fileMemory == MAP_FAILED)
    {
        perror("Error al proyectar el archivo en memoria");
        close(fd);
        exit(EXIT_FAILURE);
    }

    //printf("Esperando... justo después de mmap()\n");

    //getchar();

    // Modificar el contenido de la proyección en memoria
    char *charArray = (char *)fileMemory;
    charArray[0] = 'X'; // Cambia el primer caracter
    charArray[10] = '2';
    charArray[42] = '4';

    if (msync(fileMemory, fileStat.st_size, MS_SYNC) == -1) {
        perror("Error al sincronizar cambios con el archivo");
    }

    // Imprimir el contenido del archivo después de la modificación en memoria
    printf("Contenido del archivo usando read:\n");
    char buffer;
    while (read(fd, &buffer, 1) > 0)
    {
        putchar(buffer);
    }

    // Desproyectar el archivo en memoria con munmap
    if (munmap(fileMemory, fileStat.st_size) == -1)
    {
        perror("Error al cerrar la proyección en memoria");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Cerrar el archivo
    close(fd);

    return 0;
}
