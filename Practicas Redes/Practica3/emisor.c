#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void main(int argc, char *argv[])
{
	/*Nos aseguramos de que el número de parámetros que se le pasan al programa a través de la línea de comandos es correcto.
	En el caso de nuestro emisor UDP, deberíamos tener:
		(1) argv[0] -> Nombre del ejecutable.
		(2) argv[1] -> Puerto propio.
		(3) argv[2] -> IP del destinatario.
		(4) argv[3] -> Puerto del destinatario.*/

	if (argc < 4)
	{
		perror("Error. Introduzca los parametros adecuados en la linea de comandos\n");
		exit(EXIT_FAILURE);
	}

	/*Convertimos las cadenas de caracteres almacenadas en argv[1] y en argv[3] (número de los puertos) en enteros. Para ello, recurrimos a la función atoi()*/
	int puertoPropio = atoi(argv[1]);
	int puertoDestino = atoi(argv[3]);

	/*Los puertos utilizados deben respetar la condición de ser mayores que IPPORT_USERRESERVED; es decir, no pueden estar reservados.*/
	if (puertoPropio < IPPORT_USERRESERVED || puertoDestino < IPPORT_USERRESERVED)
	{
		perror("Error. Alguno de los puertos es incorrecto.\n");
		exit(EXIT_FAILURE);
	}

	/*1. CREACIÓN DEL SOCKET DEL EMISOR HACIENDO USO DE LA FUNCIÓN socket()
	- La función socket() crea un nuevo socket.
	- Parámetros de Entrada:
		(1) int domain: AF_INET (empleo de direcciones IPv4).
		(2) int style: SOCK_DGRAM (socket no orientado a conexión).
		(3) int protocol: 0 (valor por defecto).
	- Parámetros de Salida: La función devuelve el número entero identificador del socket en caso de éxito, y -1 en caso de error.*/

	int propioSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (propioSocket < 0)
	{
		perror("Error en la Creacion del Socket Emisor\n");
		exit(EXIT_FAILURE);
	}

	/*2. ASIGNACIÓN DE DIRECCIÓN Y PUERTO AL SOCKET A TRAVÉS DE LA FUNCIÓN bind()
	- La función bind() es utilizada para asignar dirección y puerto a un socket.
	- Parámetros de Entrada:
		(1) int socket: Entero identificador del socket.
		(2) struct sockaddr *addr: addr es un puntero a un struct sockaddr con la dirección que se le quiere asignar al socket.
			(2.1) sin_family: AF_INET
			(2.2) sin_port: Puerto propio del emisor (el puerto elegido).
			(2.3) sin_addr.s_addr: INADDR_ANY para poder aceptar peticiones que lleguen por cualquier interfaz.
		(3) socklen_t length: Tamaño de la estructura.
	- Parámetros de Salida: La función devuelve 0 en caso de éxito, y -1 en caso de error.*/	

	/*Estructura del emisor con IP y puerto propios (INADDR_ANY y el puerto elegido).*/
	struct sockaddr_in propio_addr;
	propio_addr.sin_family = AF_INET;
	propio_addr.sin_port = htons(puertoPropio); // Puerto en orden de red.

	/*La macro INADDR_ANY está en orden de host, por lo que tenemos que cambiarla a orden de red.*/
	propio_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(propioSocket, (struct sockaddr *)&propio_addr, sizeof(propio_addr)) < 0)
	{
		perror("Error en la Asignacion de Puerto y Direccion\n");
		exit(EXIT_FAILURE);
	}

	/*3. ENVÍO DEL MENSAJE UTILIZANDO LA FUNCIÓN sendto()
	- La función sendto() hace posible el envío de datos sin conexión.
	- Parámetros de Entrada:
		(1) int socket: Entero identificador del socket.
		(2) void *buffer: Puntero a los datos a enviar.
		(3) size_t size: Número de bytes a enviar.
		(4) int flags: Opciones de envío (por defecto, int flags = 0)
		(5) struct sockaddr *addr: addr es un puntero a un struct sockaddr con la dirección a la que se quieren enviar los datos.
			(5.1) sin_family
			(5.2) sin_port: Puerto remoto.
			(5.3) sin_addr.s_addr
		(6) socklen_t length: Tamaño de la estructura addr.
	- Parámetros de Salida: Número de bytes transmitidos aunque sea con errores (éxito), o -1 (error).*/

	/*Almacenamiento de la IP remota (valor de argv[2]) en una variable.*/
	char *ipDestino = argv[2];

	/*Estructura del emisor con IP y puerto remotos.*/
	struct sockaddr_in destino_addr;
	destino_addr.sin_family = AF_INET;
	destino_addr.sin_port = htons(puertoDestino); // Puerto en orden de red.

	/*Almacenamiento de la IP remota en el campo sin_addr.s_addr
	inet_pton se utiliza para convertir una dirección IP en formato legible en su formato binario correspondiente.*/
	if (inet_pton(AF_INET, ipDestino, &(destino_addr.sin_addr)) <= 0)
	{
		perror("Problema al Configurar Direccion\n");
		exit(EXIT_FAILURE);
	}

	char mensaje[] = "Buenos dias";

	/*Agregamos '\0' al final del mensaje para asegurarnos de que éste está terminando con el carácter nulo.*/
	mensaje[sizeof(mensaje) - 1] = '\0';

	/*ENVÍO DE LOS DATOS*/
	ssize_t bytesEnviados = sendto(propioSocket, mensaje, strlen(mensaje) + 1, 0, (struct sockaddr *)&destino_addr, sizeof(destino_addr));
	if (bytesEnviados < 0)
	{
		perror("Error en el Envio del Mensaje\n");
		exit(EXIT_FAILURE);
	}
	else{
		/*Impresión del Número de Bytes Enviados.*/
		printf("Se han enviado %ld bytes\n", bytesEnviados);
	}

	/* APARTADO (D)
	En este apartado debemos modificar el código para que envíe un array de enteros en lugar de un string.
	Realizamos las siguientes modificaciones en el mensaje y en sendto:

	float array[] = {3.14, 2.72, 1.56};

	//ENVÍO DE LOS DATOS
	ssize_t bytesEnviados = sendto(propioSocket, array, sizeof(array), 0, (struct sockaddr *)&destino_addr, sizeof(destino_addr));
	if (bytesEnviados < 0)
	{
		perror("Error en el Envio del Mensaje\n");
		exit(EXIT_FAILURE);
	}
	else{
		//Impresión del Número de Bytes Enviados.
		printf("Se han enviado %ld bytes\n", bytesEnviados);
	}
	*/

	/*4. FUNCIÓN close()
	- La función close() cierra un socket.
	- Parámetro de Entrada:
		(1) int socket: Entero identificador del socket que queremos cerrar.
	- Parámetro de Salida: La función close() devuelve 0 en caso de éxito, y un valor distinto del nulo en caso de error.*/

	if (close(propioSocket) != 0)
	{
		perror("Error al Cerrar un Socket");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

