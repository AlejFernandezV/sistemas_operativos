
/**
* @file
* @brief Servidor HTTP
* @author Erwin Meza Vega <emezav@unicauca.edu.co>
*/

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "util.h"

/**
* @brief Manejador de señales.
* @param num Numero de la señal recibida
* @note El manejador debe ser instalado mediante sigaction(2).
*/
void signal_handler(int num);

/** @brief Controla la ejecucion del servidor */
int finished = 0;

int main(int argc, char * argv[]) {

	//Socket del servidor
	int s;

	//Conexion del cliente
	int c;

	//Direccion IPv4 del servidor
	struct sockaddr_in addr;

	//Estructura de accion de señal
	struct sigaction act;

	//Buffer de lectura
	char buf[BUFSIZ];
	ssize_t nread;
	ssize_t nwritten;

	//Inicializar la estructura con ceros (NULL)
	memset(&act, 0, sizeof(struct sigaction));

	//Establecer el manejador de la señal
	act.sa_handler = signal_handler;
	
	//Instalar el manejador de la señal SIGINT
	sigaction(SIGINT, &act, NULL);

	//Instalar el manejador de la señal SIGTERM
	sigaction(SIGTERM, &act, NULL);

	//1. Crear un socket IPv4, flujo (stream)
	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s < 0) {
		perror("Socket");
		exit(EXIT_FAILURE);
		}


	//Inicializar la direccion antes de asociarla
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234); //TODO argumento de linea de comandos!!!
	if (inet_aton("0.0.0.0", &addr.sin_addr) != 1) { //Escuchar en la direccion 	ip QUE SE ME HAYA ASIGNADO
		fprintf(stderr, "Unable to parse the address\n");
		exit(EXIT_FAILURE);
	}

	//2. Asociar el socket a la direccion addr de tipo IPv4  (bind)
	if (bind(s, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) != 0) {
		perror("bind");
		exit(EXIT_FAILURE);
		}


	//3 Poner el socket disponible para recibir conexiones
	if (listen(s, 10) != 0) {
		perror("listen");
		exit(EXIT_FAILURE);
		}
	printf("Server started.\n");
		printf("Cuanto vale BUFSIZ? %d\n", BUFSIZ);

	//4. Esperar (BLOQUEARSE) por una conexion
	c = accept(s, NULL, 0); //TODO opcional: recibir la direccion del cliente
	if (c < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
		}

	/* TODO crear un hilo que realice la siguiente tarea: */

	while(!finished) {
		//Leer de la conexion del cliente
		nread = read(c, buf, BUFSIZ);
		if (nread <=0) {
			//End read loop
			finished = 1;
			continue;
		}
		//La solicitud tiene formato GET /ruta_archivo HTTP/1.1\n\n
		// Buscar el archivo dentro del subdirectorio www/
		//Si no existe, construir el mensaje de error (HTTP/1.1 404 Not Found) y enviar
		//En Caso contrario:
		//Encontrar el tamaño del archivo (stat)
		//Construir el encabezado
		//Enviar el encabezado seguido de \n\n
		//Enviar el contenido del archivo
		//Cerrar la conexion y terminar el hilo		
	}

	//5. Cerrar la conexion con el cliente
	close(c);

	//6. Cerrar el socket del servidor
	close(s);
	printf("Server finished.\n");

	exit(EXIT_SUCCESS);
}


void signal_handler(int num) {
	printf("Signal %d received\n", num);
	finished = 1;
}
