#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        // Este es el proceso hijo
        printf("Proceso hijo terminando...\n");
        sleep(5);
        printf("Proceso hijo terminado\n");
        exit(0);
    } else if (child_pid < 0) {
        perror("Error al crear el proceso hijo");
        exit(1);
    } else {
        // Este es el proceso padre
        sleep(10);  // Espera un tiempo sin llamar a wait
        printf("Proceso padre terminando sin llamar a wait.\n");
    }

    return 0;
}