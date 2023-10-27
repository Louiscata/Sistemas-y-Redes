#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#define MAX_SIZE 200

int main(int argc, char *argv[]) {

    if (argc!=4)
    {
        perror("Introducir direccion y puerto del servidor\n");
        exit(1);
    }

    FILE *origen;
    if ((origen = fopen(argv[1], "r")) == NULL) {
        perror("No se pudo abrir el archivo\n");
        return EXIT_FAILURE;
    }

    char archivo_destino[strlen(argv[1])];
    strcpy(archivo_destino, argv[1]);
    for(int i = 0; argv[1][i] != '.'; i++) archivo_destino[i] = toupper(archivo_destino[i]);
    
    FILE *destino;
    if ((destino = fopen(archivo_destino, "w+")) == NULL) {
        perror("No se pudo abrir el archivo\n");
        return EXIT_FAILURE;
    }

    struct in_addr direccion;
    int puerto = atoi(argv[3]);
    inet_pton(AF_INET, argv[2], &direccion);
    //Confirmamos que la direccion esté correcta
    char ip_texto[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&direccion,ip_texto,INET_ADDRSTRLEN);
    printf("Direccion: %s\nPuerto: %d\n", ip_texto, puerto);
    
    int socket_cliente;
    struct sockaddr_in direccion_socket;
    socklen_t tam_socket = sizeof(struct sockaddr_in);
    ssize_t bytes_recibidos;

    direccion_socket.sin_addr.s_addr = direccion.s_addr;
    direccion_socket.sin_family = AF_INET;
    direccion_socket.sin_port = htons(puerto);

    //Crear socket
    socket_cliente = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_cliente < 0)
    {
        perror("No se pudo crear el socket\n");
        exit(1);
    }

    //Conectar con el servidor
    if(connect(socket_cliente, (struct sockaddr *) &direccion_socket, tam_socket) < 0)
    {
        perror("No se pudo conectar con el servidor\n");
        exit (1);
    }

    //Enviar y recibir linea a linea
    char linea[MAX_SIZE];
    char buffer[MAX_SIZE];
    ssize_t bytes_enviados=0;
    while (fgets(linea, sizeof(linea), origen) != NULL) {
        
        //Ponemos strlen(linea) + 1 para enviar el '\0'
        if((bytes_enviados = send(socket_cliente, linea, strlen(linea) + 1, 0)) < 0) {
            perror("No se pudo enviar el mensaje");
            exit(1);
        }
        printf("Bytes enviados: %ld\n",bytes_enviados);

        if((bytes_recibidos = recv(socket_cliente, buffer, MAX_SIZE, 0)) < 0)
        {
            perror("Error al recibir el mensaje\n");
            exit (1);
        }
        printf("Bytes recibidos: %ld\n",bytes_recibidos);

        //printf("\nString salida: %s",buffer);
        
        fputs(buffer,destino);
    }

    //Le digo que ya termina
    send(socket_cliente, NULL, 1, 0);

    //Cerrar el socket
    close(socket_cliente);

    return (EXIT_SUCCESS);
}