
/**
* @file
* @brief Servidor
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

	//Buffer de lectura de la entrada estandar
	char inbuf[BUFSIZ];
	//Buffer para leer datos desde el socket del servidor
	char buf[BUFSIZ];

	ssize_t nread; //Count of bytes read.
	ssize_t nwritten; //count of bytes written.


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

	//Inicializar la direccion antes de conectarse
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000); //TODO argumento de linea de comandos!!!
	if (inet_aton("127.0.0.1", &addr.sin_addr) != 1) { //Conectarse a la direction TODO del servidor
		fprintf(stderr, "Unable to parse the address\n");
		exit(EXIT_FAILURE);
	}

	//2. Conectarse a la direccion addr de tipo IPv4  (bind)
	if (connect(s, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) != 0) {
		perror("bind");
		exit(EXIT_FAILURE);
		}

	//POST: Connection established.
	printf("Client connected.\n");

	while(!finished) {

        //Leer el nombre del archivo de la entrada estandar
        //Construir y enviar el mensaje GET /ruta_archivo HTTP/1.1\n\n
        //Leer la respuesta
        //Si la respuesta es 200 OK:
        //Leer el encabezado del mensaje (hasta encontrar \n\n)
        //Despues del encabezado se recibe el contenido del archivo
        //Leer el contenido del archivo y guardar dentro del directorio files/
		/*
			Crear un archivo temporal y guarde la respuesta del servidor, para luego procesar esta entrada,
			es decir, abrir el temporal y procesar su contenido.
		*/
	}

	//3. Cerrar la conexion con el servidor
	close(s);
	printf("Client finished.\n");

	exit(EXIT_SUCCESS);
}


void signal_handler(int num) {
	printf("Signal %d received\n", num);
	finished = 1;
}
