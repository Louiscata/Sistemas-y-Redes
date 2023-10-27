#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//El siguiente codigo crea un numero N de hijos

//Numero de hijos
#define N 10

void main() {

    for (int i = 0; i < N; i++) {

        pid_t pid = fork();
        
        if (pid == 0) {
            //Este codigo lo ejecuta el proceso hijo
            printf("Aqui el proceso hijo numero %d con PID %d\n", i + 1, getpid());
            //Hacemos que el hijo salga del bucle para que no cree mas hijos
            break;
        }
    }
}