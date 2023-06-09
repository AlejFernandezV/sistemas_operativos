#include <arpa/inet.h> //inet_aton, inet_ntoa, ...
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <netinet/in.h> //IPv4
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> //Sockets
#include <sys/stat.h>
#include <unistd.h>
#include "protocol.h"
#include "split.h"

/**
 * @brief Envia encabezado del archivo
 *
 * @param client_sd socket
 * @param fn nombre del archivo
 * @param s objeto para acceder al estado del archivo
 */
int enviarInfoArchivo(int client_sd, char *fn, struct stat s);

/**
 * @brief Recibe el archivo desde el servidor
 * @param sd socket
 * @param fn nombre del archivo
 * @param comand comando digitado para realizar determinada accion
 */
int recibirArchivo(int sd, char *fn, char *comand);

int finished;

int main(int argc, char *argv[])
{
	// Socket
	int sd;

	// Direccion del servidor(IPv4)
	struct sockaddr_in addr;
	char m[200];
	char comand[BUFSIZ];
	FILE *fp;

	split_list *sp;

	char *fn;
	char *path;
	struct stat s;
	char ruta[PATH_MAX];

	/* Primero se obtiene la direccion IP del host a conectar */
	if (argc != 3)
	{
		printf("Faltan parametros de llamada.\n");
		exit(EXIT_FAILURE);
	}

	char *server_addr = argv[1];
	unsigned short port = atoi(argv[2]);

	/* 1. Creando el socket con el servidor */
	// Socket IPv4, de tipo flujo(stream)
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0)
	{
		perror("sockect");
		exit(EXIT_FAILURE); // si no se puede crear el socket
	}

	// Preparar la direccion para asociarla al socket
	// formato de la direccion IPv4
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(server_addr, &addr.sin_addr);

	printf("Connecting con el servidor...\n");
	// 2. Conectarse al servidor
	connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	printf("Conectado!\n");

	finished = 0;
	// Enviar solicitud al servidor
	request req;
	memset(&req, 0, sizeof(request));
	while (!finished)
	{
		// lee una linea de la entrada estandar
		printf("\n____________________________________________________\n");
		printf("Realice su solicitud \n");
		printf(">");
		memset(comand, 0, BUFSIZ); // Rellenar con ceros el bufer
		fgets(comand, 80, stdin);  /*Leer comando por entrada estandar*/

		sp = split(comand, " \r\n");

		// Linea vacia? repetir
		if (sp->count == 0)
		{
			continue;
		}
		if (strcmp(sp->parts[0], "exit") != 0 && strcmp(sp->parts[0], "get") != 0)
		{
			continue;
		}
		// struct stat s;
		if ((strcmp(sp->parts[0], "exit") == 0) || (strcmp(sp->parts[0], "EXIT") == 0))
		{
			strcpy(req.comando, sp->parts[0]);
			write(sd, &req, sizeof(request));
			close(sd);
			finished = 1;
		}
		else if (((strcmp(sp->parts[0], "get") == 0) || (strcmp(sp->parts[0], "GET") == 0)) && sp->count == 2)
		{
			strcpy(req.comando, sp->parts[0]);
			strcpy(req.filename, sp->parts[1]);

			strcpy(ruta, "www/");
			strcat(ruta, req.filename);
			path = realpath(ruta, NULL);

			// enviar la solicitud
			write(sd, &req, sizeof(request));
			recibirArchivo(sd, req.filename, req.comando);
		}
		else
		{
			printf("Comando no valido!\n");
			continue;
		}
	}
}

int recibirArchivo(int sd, char *fn, char *comand)
{
	file_info infoF;
	int escritos;
	int leido;
	int out_fd;
	int faltantes;
	int a_leer;
	int nread;
	int escritos2;
	char out_filename[PATH_MAX];
	char out_filename_error[PATH_MAX];
	char *buffer;
	char buf[BUFSIZ];
	char buf_error[BUFSIZ];

	// Limpia la estructura
	memset(&infoF, 0, sizeof(file_info));

	strcpy(infoF.filename, fn);
	strcpy(infoF.comando, comand);

	printf("\nArchivo a recibir del servidor: %s\n", infoF.filename);

	// Envia el protocolo al servidor
	escritos = write(sd, &infoF, sizeof(file_info));
	if (escritos <= 0)
	{
		printf("Fallo enviar el encabezado del archivo\n");
		exit(EXIT_FAILURE);
	}

	memset(&infoF, 0, sizeof(file_info));
	// Lee el encabezado que envia el servidor
	leido = read(sd, &infoF, sizeof(infoF));

	// Verifico que el tamaño del archivo enviado por el servidor sea mayor a cero.
	if (infoF.size <= 0)
	{
		printf("El archivo no existe en el servidor...\n");
	}
	printf("Leyendo archivo enviado por el servidor...\n");
	printf("%s", &infoF.mensaje);
	if (strcmp(infoF.filename, "error.html") == 0)
	{
		// // Leer el contenido del archivo
		strcpy(out_filename_error, "www/");
		strcat(out_filename_error, "error.html");
		FILE *file = fopen(out_filename_error, "r");
		// Leer el contenido del archivo
		while (fgets(buf_error, sizeof(buf_error), file) != NULL)
		{
			printf("%s", buf_error);
		}
	}
	else
	{
		// Leer el contenido del archivo
		strcpy(out_filename, "files/");
		strcat(out_filename, infoF.filename);
		FILE *file = fopen(out_filename, "r");
		// Leer el contenido del archivo
		while (fgets(buf, sizeof(buf), file) != NULL)
		{
			printf("%s", buf);
		}
	}

	out_fd = open(out_filename, O_CREAT | O_WRONLY, infoF.mode);

	faltantes = infoF.size;

	while (faltantes > 0)
	{
		// suponer que todavia quedan suficientes bytes a leer
		a_leer = BUFSIZ;
		// Verificar si quedan menos bytes por leer en el archivo
		if (faltantes < BUFSIZ)
		{
			a_leer = faltantes;
		}
		memset(buf, 0, BUFSIZ);
		// Lee del socket el contenido del archivo enviado por el servidor y lo manda al buffer
		nread = read(sd, buf, a_leer);
		if (nread > 0)
		{
			printf("%s", buf);
			// Envia el contenido del buffer al archivo creado
			escritos2 = write(out_fd, buf, nread);
			if (escritos2 > 0)
			{
				faltantes = faltantes - nread;
			}
			else
			{
				printf("Fallo escribir en el archivo\n");
			}
		}
		else
		{
			break;
		}
	}
	close(out_fd);
}

int enviarInfoArchivo(int client_sd, char *fn, struct stat s)
{
	file_info infoF;
	int escritos;
	// Limpia la estructura
	memset(&infoF, 0, sizeof(file_info));
	// Asignando valores a la estructura
	strcpy(infoF.filename, fn);
	infoF.size = s.st_size;
	infoF.mode = s.st_mode;

	// Envia infoF con la info del archivo
	escritos = write(client_sd, &infoF, sizeof(file_info));
	if (escritos <= 0)
	{
		return infoF.size;
		printf("Fallo enviar la informacion del archivo al cliente.");
	}
	return infoF.size;
}