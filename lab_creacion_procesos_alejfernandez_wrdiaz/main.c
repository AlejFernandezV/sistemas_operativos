/**
 * @file
 * @brief Creacion de procesos
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>

#include "split.h"

#define EQUALS(s1,s2) (strcmp(s1,s2)==0)

/*
	@brief Prueba de la subrutina split	
*/

int main(int argc, char * argv[]){
	int finished = 0;
	char linea[BUFSIZ];
	int len;
	int pid;
	split_list * list;

	while(!finished){
		printf("$ ");

		//Limpiar la linea
		memset(linea, BUFSIZ, 0);

		//Leer una linea
		if(fgets(linea, BUFSIZ, stdin)==NULL){
			finished = 1;
			continue;
		}
		//POST: Linea leida
		len = strlen(linea); 
		if(len <= 1){
			//Repetir la lectura de la linea
			continue;
		}
		//POST: La linea contiene al menos un caracter
		//Quitar en NEWLINE al final
		if(linea[len-1] == '\n'){
			linea[len-1] = 0;
			len --;
		}
		//POST: La linea es un comentario
		if(linea[0] == '#'){
			continue;
		}

		//Partir la linea
		list = split(linea, " \t\n|");

		if(EQUALS(list->parts[0],"exit") || EQUALS(list->parts[0],"quit")){
			//Marcar la bandera de terminacion y repetir el ciclo
			finished = 1;
			continue;
		}

		//Se crea una copia
		pid = fork();
		
		if(pid == -1){
			perror("fork");
			continue;
		}

		//Proceso padre: Esperará a que el hijo termine
		if(pid > 0){
			waitpid(pid,0,0);
		}
		else{
			//Copia: Ejecutar el comando que se encuentra en list->parts
			execvp(list->parts[0], list->parts);
			//Si execvp retorna, el comando no se pudo ejecutar
			//Imprimir el mensaje de error de la llamada al sistema execvp
			perror(list->parts[0]);
			//Terminar inmediatamente 
			exit(EXIT_FAILURE);
		}
	}


	exit(EXIT_SUCCESS);
}

void test_split() {
	split_list * list;
	int i;

	list = split(" hola mundo esta es una cadena     adios", " \t\n|");

	for (i= 0; i< list->count; i++) {
		printf("#%s# ", list->parts[i]);
	}
}
