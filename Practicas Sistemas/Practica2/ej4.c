#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// NO EJECUTAR XDDD

int main() {
    pid_t hijo1, hijo2;

    // Crear el primer hijo
    if ((hijo1 = fork()) == 0) {
        // Proceso hijo 1: se convierte en zombie inmediatamente
        printf("Hijo 1 (Zombie) con PID: %d\n", getpid());
        exit(0);
    } else if (hijo1 < 0) {
        perror("Error al crear el primer hijo");
        exit(1);
    }

    // Esperar un breve momento para que el hijo 1 se convierta en zombie
    sleep(2);

    // Crear el segundo hijo
    if ((hijo2 = fork()) == 0) {
        // Proceso hijo 2: se queda huérfano y se ejecuta durante un tiempo prolongado
        printf("Hijo 2 (Huerfano) con PID: %d\n", getpid());
        while (1) {
            // Mantener al hijo 2 en ejecución
        }
    } else if (hijo2 < 0) {
        perror("Error al crear el segundo hijo");
        exit(1);
    }

    // Padre espera a que el hijo 1 termine y recoge el valor de salida
    int estado;
    waitpid(hijo1, &estado, 0);
    if (WIFEXITED(estado)) {
        int exit_estado = WEXITSTATUS(estado);
        printf("El hijo 1 termino con valor de salida: %d\n", exit_estado);
    }

    // El padre sigue ejecutándose después de que el hijo 1 termina
    printf("Padre con PID: %d terminando\n", getpid());
    
    return 0;
}