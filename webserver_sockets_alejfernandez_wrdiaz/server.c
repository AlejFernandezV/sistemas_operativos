#include <arpa/inet.h> //inet_aton, inet_ntoa, ...
#include <fcntl.h>
#include <limits.h>
#include <libgen.h>
#include <netinet/in.h> //IPv4
#include <pthread.h> //Hilos
#include <semaphore.h> //Semaforos
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "protocol.h"
#include "split.h"

#define MAX_CLIENTES 256
typedef sem_t semaphore;
semaphore mutex;
int finished;
int clients[MAX_CLIENTES];

/**
 * @brief decrementa el valor del semaforo
 * @param s apuntador a un semaforo
 */
void down(semaphore * s);
/**
 * @brief incrementa el valor del semaforo 
 * @param s apuntador a un semaforo
 */
void up(semaphore * s);


int recibirArchivo(int client_sd,file_info infoF);
/**
 * @brief envia el archivo solicitado por el cliente
 * @param client_sd 
 * @param infoF instancia de la estructura file_info
 */
int transferirArchivo(int client_sd,file_info infoF);

/**
 * @brief envia la informacion del archivo solicitado por el cliente
 * @param client_sd 
 * @param fn nombre del archivo
 * @param mensaje de error o de ok
 * @param s objeto para acceder al estado del archivo
 */
int enviarInfoArchivo(int client_sd,char* fn,int band,struct stat s);

/**
 * @brief proceso que se realiza cada vez que se conecta un nuevo cliente
 * @param client_sd apuntador al socket
 */
void * atender_cliente(void * client_sd);

/**
 * @brief termina la ejecucion del programa al recibir la señal 
 * @param signal señal recibida
 */
void handle_sigterm(int signal);

/**
 * @brief agrega el indice relacionado con el cliente que se esta conectando a un arreglo
 * para poder gestionar los clientes conectados
 * @param c entero asociado al cliente que se encuentra conectado
 */
int agregar_cliente(int c);

/**
 * @brief Arma el mensaje que llegará al cliente
 * @param title Será el mensaje de error o de ok respectivamente
 * @param fi estructura que guardará la informacion del archivo
 */
char* mensajeSalida(char * title, int size, char* extension);

/**
 * @brief programa principal
 * @param argc cantidad de argumentos que se ingresan por linea de comandos
 * @param argv argumentos 
 */
int main(int argc,char *argv[])
{
   struct sigaction act; 
   struct sigaction oldact; 
   struct stat st = {0};   
   int sd; //socket servidor
   int client_sd; //socket cliente

   //Direccion del servidor(IPv4)
   struct sockaddr_in addr;
   unsigned short port; //puerto es un entero sin signo
   char buf[1024];
   char* path;
   file_info infoF;
   request req;
   int leido;
   int leido2;
   finished=0;
   /*Manejador de señales*/
    memset(&act, 0, sizeof(struct sigaction));
    memset(&oldact, 0, sizeof(struct sigaction));
    /*Cuando se reciba  SIGTERM se ejecutará handle_sigterm*/
    act.sa_handler = handle_sigterm;
    /*Instalamos el manejador para SIGTERM*/
    sigaction(SIGTERM, &act, &oldact);
    /*Instalamos el manejador para SIGINT*/
    sigaction(SIGINT, &act, &oldact);
	
	//Crear carpeta files
   if (stat("files", &st) == -1){
		//se crea el directorio files con permisos
		mkdir("files", 0755);  
	}
	if (argc != 2) {
    	fprintf(stderr, "Debe especificar el puerto a escuchar\n");
    	exit(EXIT_FAILURE);
 	}

  	/* 1.Creando el socket con el servidor */
	//Socket IPv4, de tipo flujo(stream)
  	sd = socket(AF_INET, SOCK_STREAM, 0);

	if(sd<0){
		perror("sockect");
		exit(EXIT_FAILURE); //si no se puede crear el socket
	}

	//Preparar la direccion para asociarla al socket
	//formato de la direccion IPv4
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	
	port = atoi(argv[1]);
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY; //0.0.0.0, cualquier direccion activa
	
	//2. Asociar el socket a una direccion (IPv4)
	printf("Asociando el socket..\n");
	bind(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

  	/* Escuchando el socket abierto */
  	listen(sd, 10);

	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(struct sockaddr_in));
	socklen_t client_addr_len;
	client_addr_len = sizeof(struct sockaddr_in); //Tamaño esperado de la direccion

  	printf("Servidor escuchando en el puerto %s\n", argv[1]);

  	/*Inicializar el mutex*/
  	sem_init(&mutex,0,1);
  
  	/* Se pasa a atender todas las peticiones hasta que se finalice el proceso */
  	while (!finished){
		//4. Aceptar la conexion
		//el sistema "llena" client_addr con la informacion del cliente
		//y almacena en client_addr_len el tamaño obtenido de esa direccion
		printf("Aceptando conexiones...\n");
		/*Bloquearse esperando a que un cliente se conecte*/
		client_sd = accept(sd, (struct sockaddr *)&client_addr, &client_addr_len);
		down(&mutex);
		int pos = agregar_cliente(client_sd);
		up(&mutex);
		pthread_t t_cliente;
		pthread_create(&t_cliente, NULL, atender_cliente, &clients[pos]);
		printf("Cliente conectado!\n");
  	}
}

void down(semaphore * s){
	sem_wait(s);
}

void up(semaphore * s){
	sem_post(s);
}

int recibirArchivo(int client_sd,file_info infoF){	
	struct stat s;
	int faltantes;
	char buf[BUFSIZ];
	int a_leer;
	int leido;
	char out_filename[PATH_MAX];
	int out_fd;
	int escritos;
	int leido2;

	memset(&infoF,0,sizeof(file_info));
	leido2 = read(client_sd, (char *)&infoF, sizeof(file_info));
	//1. Recibir la informacion del archivo	
	// Inmediatamente despues del infoF, leer el contenido del archivo
	strcpy(out_filename, "www/");
	strcat(out_filename, infoF.filename);
	out_fd = open(out_filename, O_CREAT | O_WRONLY , infoF.mode); 
	
	faltantes = infoF.size;
	
	while(faltantes > 0) {
		//suponer que todavia quedan suficientes bytes a leer
		a_leer = BUFSIZ;
		//Verificar si quedan menos bytes por leer en el archivo
		if (faltantes < BUFSIZ) {
		    a_leer = faltantes;		    
		}
		memset(buf,0,BUFSIZ);
		printf("Bytes a leer %d\n",a_leer);
		leido = read(client_sd, buf, a_leer);
		printf("Bytes leidos %d\n",leido);
		if(leido>0){
			printf("Contenido del archivo: %s\n",buf);
			escritos=write(out_fd, buf, leido);
			if(escritos>0){
			   faltantes = faltantes - leido;
			}else{
			   printf("Fallo escribir en el archivo");
			}				
		}else{
			break;
		}		
	}
	close(out_fd);	
}

int transferirArchivo(int client_sd,file_info infoF){
  	char ruta[PATH_MAX];
  	char* path;
  	char* fn;
  	char buf[BUFSIZ];
  	struct stat s;
  	int f;
	int faltantes;
	int a_leer;
	int leidos, leido2;	
	int escritos;
	int size;
		
  	strcpy(ruta,"www/");
	strcat(ruta, infoF.filename);
	fn=infoF.filename;
	
	memset(&infoF,0,sizeof(file_info));
	leido2 = read(client_sd, (char *)&infoF, sizeof(file_info));

	printf("Nombre del archivo a enviar al cliente: %s\n",fn);
	
	path=realpath(ruta,NULL);
		     	
	if (path == NULL) {
	    printf("No existe el archivo o directorio solicitado por el cliente\n");
	    //Enviando infoF con la información del archivo solicitado, al cliente.
	    size=enviarInfoArchivo(client_sd,"error.html",0,s);     	    	  
	}
		
	if (stat(path, &s) != 0) {
	    perror("stat");
	}
	if (!S_ISREG(s.st_mode)) {
	   printf("%s El cliente ha solicitado un directorio,No un archivo!\n", path);		  
	}

	//Enviando infoF con la información del archivo solicitado, al cliente.	
	size=enviarInfoArchivo(client_sd,fn,1,s);

	printf("Enviando archivo al cliente...\n");
	//abrir el archivo modo lectura
	f=open(ruta,O_RDONLY);
		 
	faltantes = size;
			 
	while(faltantes > 0) {
	        
		a_leer = BUFSIZ;
		//Verificar si quedan menos bytes por leer en el archivo
		if (faltantes < BUFSIZ) {
		    a_leer = faltantes;
		}
		memset(buf,0,BUFSIZ);
		printf("Bytes a leer: %d\n",a_leer);
		leidos = read(f, buf, a_leer);	
		printf("Bytes leidos: %d\n",leidos);
		if(leidos>0){
		   escritos=write(client_sd, buf, leidos);
		   if(escritos>0){
		       faltantes = faltantes - leidos;	
		   }else{
			printf("Fallo enviar al servidor");
		   }
		}else{
			printf("No se pudo leer el contenido del archivo.\n");
			break;
		}
	}
	printf("Transferencia completa...\n");
	close(f); 		
}

int enviarInfoArchivo(int client_sd,char* fn,int band,struct stat s){
 	file_info infoF;
	char* mensaje;
	char* extension;
 	int escritos;

 	//Limpia la estructura
	memset(&infoF,0,sizeof(file_info));
	//Asignando valores a la estructura
	strcpy(infoF.filename,fn);
	infoF.size = s.st_size;
	infoF.mode = s.st_mode;
	
	extension = strchr(infoF.filename,46);

	if(extension == NULL){
		extension = "NO EXTENSION";
	}

	if(band==1){
		mensaje = mensajeSalida("200 OK",infoF.size,extension);
	}
	else{
		mensaje = mensajeSalida("404 NOT FOUND",infoF.size,extension);
	}

	strcpy(infoF.mensaje,mensaje);
	//printf("Mensaje \n%s", infoF.mensaje);
	
	//Envia infoF al cliente con la info del archivo
	escritos = write(client_sd,&infoF,sizeof(file_info));
	if(escritos <= 0){
		return infoF.size;
	    printf("Fallo enviar la informacion del archivo al cliente.");
	}
	return infoF.size;	
}

void * atender_cliente(void * client_sd){
	int cliente = *(int *)client_sd;
    int leido,leido2;
	file_info infoF;
	request req;
	int client_finished;
	
	client_finished  = 0;  
	//validar si este cliente termino (client_finished) y si todo el servidor se detuvo (finished)
    while(!client_finished || !finished){   
	    
	    memset(&req,0,sizeof(request));
	    //Leer el infoF para obtener la informacion del archivo del socket
	    leido=read(cliente, &req, sizeof(request));
		//printf("Obteniendo del cliente: %s %s\n", req.comando, req.filename);
		strcpy(infoF.filename, req.filename);
		strcpy(infoF.comando, req.comando);		
		
	    if(leido <= 0){
			perror("read");
	    	break;
	    }

		if((strcmp(req.comando,"exit") == 0) || (strcmp(req.comando,"EXIT") == 0)){
			printf("Conexion terminada\n");  
			close(cliente);
			client_finished=1;  
	    }else if((strcmp(req.comando,"get") == 0) || (strcmp(req.comando,"GET") == 0)){	
			transferirArchivo(cliente, infoF);
		}else{
			printf("Comando no valido!\n");	
	    }	    
    }    
}

void handle_sigterm(int signal){
    printf("Servidor finalizado\n");
    finished = 1;
    /*Cerrar todos los recursos abiertos*/
    fclose(stdin);
}

int agregar_cliente(int c){
	memset(clients,0,MAX_CLIENTES*sizeof(int));
	for(int i=0; i<MAX_CLIENTES;i++){
		if(clients[i]==0){
			clients[i]=c;
			return i;
		}
	}
}

char* mensajeSalida(char * title, int size, char* extension){
	int * psize = &size;
	
	// Create the message header.
	char * message = malloc(1024);
	// Get the current time.
  	time_t now = time(NULL);

  	//convertir el tiempo actual a la estructura tm.
  	struct tm *local_time = localtime(&now);

	sprintf(message,
			"\nHTTP/1.1 %s\r\n"
			"X-Powered-By: OS HTTP Server\r\n"
			"Content-Type: %s\r\n"
			"Content-Length: %d\r\n"
			"Date: %d-%02d-%02d\r\n\r\n",
			title, extension , psize, local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday);

	// Return the message.
	return message;
}