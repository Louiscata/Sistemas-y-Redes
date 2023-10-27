#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_SIZE 200

int main(int argc, char *argv[]) {

    if (argc!=2)
    {
        perror("Introducir Puerto\n");
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[1]);
    printf("Puerto %d\n", puerto);
    
    int socket_servidor, socket_conexion;
    struct sockaddr_in direccion_socket_servidor, direccion_socket_cliente;
    socklen_t tam_socket_servidor, tam_socket_cliente;
    ssize_t bytes_enviados, bytes_recibidos;

    tam_socket_servidor = sizeof(struct sockaddr_in);
    tam_socket_cliente = sizeof(struct sockaddr_in);

    direccion_socket_servidor.sin_addr.s_addr=htonl(INADDR_ANY);
    direccion_socket_servidor.sin_family=AF_INET;
    direccion_socket_servidor.sin_port=htons(puerto);

    //Crear socket
    socket_servidor = socket(AF_INET, SOCK_STREAM,0);
    if (socket_servidor < 0)
    {
        perror("No se pudo crear el socket\n");
        exit(1);
    }
    
    //Bindear socket
    if(bind(socket_servidor, (struct sockaddr *) &direccion_socket_servidor, tam_socket_servidor) < 0) 
    {
        perror("No se pudo asignar direccion al socket\n") ;
        exit (1) ;
    }

    //Poner el socket en pasivo para que pueda recibir peticiones de conexion
    //Un maximo de tres peticiónes a la vez
    if(listen(socket_servidor, 1) < 0)
    {
        perror("No se pudo poner el socket en pasivo");
        exit (1);
    }

    while (1)
    {
        //Aceptar la peticion del cliente
        if((socket_conexion = accept(socket_servidor, (struct sockaddr*) &direccion_socket_cliente, &tam_socket_cliente)) < 0)
        {
            perror("No se pudo aceptar la conexion");
            exit(1);
        }
        
        //Mostrar la ip y el puerto del cliente
        char ip_cliente_texto[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &direccion_socket_cliente.sin_addr.s_addr, ip_cliente_texto, INET_ADDRSTRLEN);
        printf("IP del cliente: %s\nPuerto del cliente: %d\n",
            ip_cliente_texto, 
            ntohs(direccion_socket_cliente.sin_port));

        char buffer[MAX_SIZE];
        while (1)
        {
            //Recibir mensaje del cliente
            
            if((bytes_recibidos = recv(socket_conexion, buffer, MAX_SIZE, 0)) < 0)
            {
                perror("Error al recibir el mensaje\n");
                exit (1);
            }

            if (buffer == NULL || bytes_recibidos == 0) break;

            //printf("Mensaje recibido: [%s]\n",buffer);
            printf("Bytes recibidos: %ld\n",bytes_recibidos);

            //Convertir el mensaje a mayúsculas
            char mensaje[bytes_recibidos];

            printf("Convirtinedo mensaje...\n");
            for(int i = 0; i < bytes_recibidos + 1; i++)
            {
                mensaje[i] = toupper(buffer[i]);
            }

            //Enviar mensaje al cliente
            if((bytes_enviados = send(socket_conexion, mensaje, strlen(mensaje) + 1, 0)) < 0) {
                perror("No se pudo enviar el mensaje\n");
                exit(1);
            }

            //Mostrar el numero de bytes enviados
            printf("Se han enviado %ld bytes\n\n", bytes_enviados);
        }

        //Cerrar los sockets
        close(socket_conexion);
    }

    return (EXIT_SUCCESS);
}