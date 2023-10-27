#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
    ssize_t bytes_enviados;

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
    //Un maximo de tres peticiones a la vez
    if(listen(socket_servidor, 3) < 0)
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

        //Enviar mensaje al cliente
        char* mensaje = "Primer mensaje pero largo";
        if((bytes_enviados = send(socket_conexion, mensaje, strlen(mensaje), 0)) < 0) {
            perror("No se pudo enviar el mensaje");
            exit(1);
        }
    
        //Apartado c: enviamos un segundo mensaje al cliente, y vemos si puede recibir ambos con un send
        //sleep(1);

        char *mensaje2 = "Segundo mensaje";
        if((bytes_enviados += send(socket_conexion, mensaje2, strlen(mensaje2) + 1, 0)) < 0) {
            perror("No se pudo enviar el mensaje");
            exit(1);
        }

        //Mostrar el numero de bytes enviados
        printf("Se han enviado %ld bytes\n", bytes_enviados);

        //Cerrar los sockets
        close(socket_conexion);
    }

    return (EXIT_SUCCESS);
}