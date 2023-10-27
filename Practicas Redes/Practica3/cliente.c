#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_SIZE 1000

int main(int argc, char *argv[]) {

    if (argc!=3)
    {
        perror("Introducir direccion y puerto del servidor\n");
        exit(1);
    }

    struct in_addr direccion;
    int puerto = atoi(argv[2]);
    inet_pton(AF_INET, argv[1], &direccion);
    //Confirmamos que la direccion esté correcta
    char ip_texto[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&direccion,ip_texto,INET_ADDRSTRLEN);
    printf("Direccion: %s\nPuerto: %d\n", ip_texto, puerto);
    
    int socket_cliente;
    struct sockaddr_in direccion_socket;
    socklen_t tam_socket = sizeof(struct sockaddr_in);
    char buffer[MAX_SIZE];
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

    //Apartado c: hacemos que el cliente espere para que el servidor pueda enviar dos mensajes
    //sleep(1);

    //Recibir el mensaje del servidor
    if((bytes_recibidos = recv(socket_cliente, buffer, MAX_SIZE, 0)) < 0)
    {
        perror("Error al recibir el mensaje\n");
        exit (1);
    }
    printf("Mensaje: %s\n",buffer);
    printf("Bytes recibidos: %ld\n",bytes_recibidos);

    //Observamos que, con espera de un segundo, el cliente recibe exitosamente ambos mensajes
    //en el mismo buffer. El número total de bytes recibidos es la suma de ambos mensajes.

    //Al eliminar la espera, observamos que el resultado puede variar. A veces se reciben ambos mensajes
    //y a veces no. Sin embargo, poniendo una espera en el servidor, el cliente solo recibe el primer
    //mensaje, y el número de bytes recibidos es el del primer mensaje. Sin embargo, el servidor
    //informa de que se han enviado los bytes de los dos mensajes.

    //Apartado d: usamos un bucle de tipo while() con la función recv(). Para ello comentamos el recv() anterior

    /* while((bytes_recibidos = recv(socket_cliente, buffer, 3, 0)) > 0 )
    {
        printf("Mensaje: %s\n", buffer);
        printf("Bytes recibidos: %ld\n", bytes_recibidos);
    } */

    //Observamos que los mensajes se fragmentan en partes de 3 caracteres cada uno, ya que hemos puesto
    //que el número máximo de bytes a recibir debe ser 3. Parece que los mensajes se quedan en cola esperando
    //a que el cliente ejecute un recv(), y cuando esto ocurre se reciben los siguientes 3 caracteres.

    //Cerramos el socket
    close(socket_cliente);

    return (EXIT_SUCCESS);
}