#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid_padre, pid_hijo;
    int a, estado;
    FILE* fp;

    a = 0;

    // Conseguir el id del proceso
    pid_padre = getpid();

    // Abrimos un fichero
    if ((fp = fopen("ejemplo.txt", "r+")) == NULL) {
        perror("Error al abrir el fichero");
        return 0;
    }

    // Crear el hijo
    pid_hijo = fork();

    if (pid_hijo == -1) {
        // Fallo al crear el hijo
        perror("Error en fork");
        return 1;
    } else if (pid_hijo == 0) {
        // Este codigo lo ejecuta el hijo
        printf("Este lo ejecuta el hijo. PID del hijo:  %d. PID del padre: %d\n", getpid(), getppid());
        printf("[Hijo] Dirección de memoria de a: %p. Valor de a: %d\n", &a, a);
        a = 1;
        printf("[Hijo] Dirección de memoria de a: %p. Valor de a: %d\n", &a, a);
        fprintf(fp, "Darth Vader!\n");
    } else {
        // Este codigo lo ejecuta el padre
        printf("Esto lo ejecuta el padre. PID del padre: %d, PID del hijo: %d\n", pid_padre, pid_hijo);
        wait(&estado);
        printf("[Padre] Dirección de memoria de a: %p. Valor de a: %d\n", &a, a);
        fprintf(fp, "Yo soy tu padre\n");
    }

    fclose(fp);

    return 0;
}