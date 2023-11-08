#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TAM 1000

int main(int argc, char *argv[])
{
	/*Nos aseguramos de que el número de parámetros que se le pasan al programa a través de la línea de comandos es correcto.
	En el caso de nuestro receptor UDP, deberíamos tener:
		(1) argv[0] -> Nombre del ejecutable.
		(2) argv[1] -> Puerto.*/

	if (argc < 2)
	{
		perror("Error. Introduzca los parametros adecuados en la linea de comandos\n");
		exit(EXIT_FAILURE);
	}

	/*Convertimos la cadena de caracteres almacenada en argv[1] (número del puerto) en un entero. Para ello, recurrimos a la función atoi()*/
	int puerto = atoi(argv[1]);

	/*El puerto utilizado debe respetar la condición de ser mayor que IPPORT_USERRESERVED; es decir, no pueden estar reservados.*/
	if (puerto < IPPORT_USERRESERVED)
	{
		perror("Error. Puerto incorrecto.\n");
		exit(EXIT_FAILURE);
	}

	/*1. CREACIÓN DEL SOCKET DEL RECEPTOR HACIENDO USO DE LA FUNCIÓN socket()
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
			(2.2) sin_port: Puerto propio del receptor (el puerto elegido).
			(2.3) sin_addr.s_addr: INADDR_ANY para poder aceptar peticiones que lleguen por cualquier interfaz.
		(3) socklen_t length: Tamaño de la estructura.
	- Parámetros de Salida: La función devuelve 0 en caso de éxito, y -1 en caso de error.*/

	/*Estructura del emisor con IP y puerto propios (INADDR_ANY y el puerto elegido).*/
	struct sockaddr_in propio_addr;
	propio_addr.sin_family = AF_INET;
	propio_addr.sin_port = htons(puerto); // Puerto en orden de red.

	/*La macro INADDR_ANY está en orden de host, por lo que tenemos que cambiarla a orden de red.*/
	propio_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(propioSocket, (struct sockaddr *)&propio_addr, sizeof(propio_addr)) < 0)
	{
		perror("Error en la Asignacion de Puerto y Direccion\n");
		exit(EXIT_FAILURE);
	}

	/*3. RECEPCIÓN DEL MENSAJE UTILIZANDO LA FUNCIÓN recvfrom()
	- La función recvfrom() hace posible la recepción de datos sin conexión.
	- Parámetros de Entrada:
		(1) int socket: Entero identificador del socket.
		(2) void *buffer: Puntero donde se van a recibir los datos.
		(3) size_t size: Número máximo de bytes a recibir.
		(4) int flags: Opciones de recepción (por defecto, int flags = 0).
		(5) struct sockaddr *addr: addr salida es un puntero a un struct sockaddr con la dirección de la procedencia del paquete.
		(6) socklen_t *length_ptr: length_ptr es un puntero que es parámetro de entrada indicando el tamaño de la estructura addr, y de salida con el espacio real consumido.
	- Parámetros de Salida: La función devuelve el número de bytes recibidos (éxito) o -1 (error).*/

	while (1)
	{
		/*Estructura del emisor con IP y puerto remotos. No es necesario inicializarla.*/
		struct sockaddr_in remoto_addr;
		socklen_t remoto_len = sizeof(remoto_addr);

		char buffer[TAM];

		/*RECEPCIÓN DEL MENSAJE*/
		ssize_t bytesRecibidos = recvfrom(propioSocket, buffer, TAM, 0, (struct sockaddr *)&remoto_addr, &remoto_len);
		if (bytesRecibidos <= 0)
		{
			perror("Error al Recibir el mensaje\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			/*Declaramos una variable para almacenar la dirección IP del que envía. El tamaño del buffer debe ser de, por lo menos, INET_ADDRSTRLEN bytes.*/
			char remotoIP[INET_ADDRSTRLEN];

			/*Almacenamos remotoIP en el campo correspondiente dentro de la estructura.
			inet_ntop toma una dirección IP en su formato binario y la convierte en una cadena legible por humanos.*/
			if ((inet_ntop(AF_INET, &(remoto_addr.sin_addr), remotoIP, INET_ADDRSTRLEN)) == NULL)
			{
				perror("Error en el tratamiento de la IP\n");
				exit(EXIT_FAILURE);
			}

			/*Imprimimos: La IP y el puerto del que envía, el número de bytes recibidos y el mensaje leído.*/
			printf("IP [%s] y puerto [%d]\n", remotoIP, ntohs(remoto_addr.sin_port));
			printf("Mensaje recibido: [%s]\n", buffer);
			printf("Se han recibido %ld bytes\n", bytesRecibidos);
		}

		/* APARTADO (C)
		En este apartado cambiamos los parámentros de la función recvfrom para que lea menos datos de los que recibe:

		ssize_t bytesRecibidos = recvfrom(propioSocket, buffer, 8, 0, (struct sockaddr *)&remoto_addr, &remoto_len);
		if (bytesRecibidos <= 0)
		{
			perror("Error al Recibir el mensaje\n");
			exit(EXIT_FAILURE);
		}
		else{...}

		Lo que observamos es que, como hemos hecho que recvfrom reciba 8 bytes, los primeros 8 caracteres se reciben
		bien. Sin embargo, se imprime el mensaje [Buenos dXD�z]. Es decir, se imprimen 12 caracteres pero los 4
		últimos son erróneos. Veamos ahora si podemos recuperar los datos perdidos con otro recvfrom:

		ssize_t bytesRecibidos = recvfrom(propioSocket, buffer, TAM, 0, (struct sockaddr *)&remoto_addr, &remoto_len);
		if (bytesRecibidos <= 0)
		{
			perror("Error al Recibir el mensaje\n");
			exit(EXIT_FAILURE);
		}
		else{...}

		A diferencia de en TCP, que podíamos recibir el mensaje en fragmentos con varios recv, en UDP no podemos
		ejecutar otro recvfrom para recuperar la información perdida. Al ejecutar el segundo recvfrom, el programa 
		se queda esperando a recibir otro mensaje, y no ocurre nada.
		*/

		/* APARTADO (D)
		En este apartado debemos modificar el código para que reciba un array de enteros en lugar de un string.
		Realizamos las siguientes modificaciones en el mensaje y en recvfrom:

		float array[TAM * sizeof(float)];

		/*RECEPCIÓN DEL MENSAJE
		ssize_t bytesRecibidos = recvfrom(propioSocket, array, TAM * sizeof(float), 0, (struct sockaddr *)&remoto_addr, &remoto_len);
		if (bytesRecibidos <= 0)
		{
			perror("Error al Recibir el mensaje\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			/*Declaramos una variable para almacenar la dirección IP del que envía. El tamaño del buffer debe ser de, por lo menos, INET_ADDRSTRLEN bytes.
			char remotoIP[INET_ADDRSTRLEN];

			/*Almacenamos remotoIP en el campo correspondiente dentro de la estructura.
			inet_ntop toma una dirección IP en su formato binario y la convierte en una cadena legible por humanos.
			if ((inet_ntop(AF_INET, &(remoto_addr.sin_addr), remotoIP, INET_ADDRSTRLEN)) == NULL)
			{
				perror("Error en el tratamiento de la IP\n");
				exit(EXIT_FAILURE);
			}

			/*Imprimimos: La IP y el puerto del que envía, el número de bytes recibidos y el mensaje leído.
			printf("IP [%s] y puerto [%d]\n", remotoIP, ntohs(remoto_addr.sin_port));
			for (int i = 0; i < bytesRecibidos / 4; i++){
				printf("Mensaje recibido: [%f]\n", array[i]);
			}
			printf("Se han recibido %ld bytes\n", bytesRecibidos);
		}*/
	}

	/*4. FUNCIÓN close()
	- La función close() cierra un socket.
	- Parámetro de Entrada: Entero identificador del socket que queremos cerrar.
	- Parámetro de Salida: La función close() devuelve 0 en caso de éxito, y un valor distinto del nulo en caso de error.*/

	if (close(propioSocket) != 0)
	{
		perror("Error al Cerrar un Socket");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

