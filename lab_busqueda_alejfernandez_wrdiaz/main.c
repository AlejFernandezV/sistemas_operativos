/**
 * @file
 * @brief Busqueda de archivos.
 * @author Alejandro Fernandez y William Diaz Maca
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/**
 * @brief Muestra la ayuda del programa
*/

void usage(char * programName);

/**
 * @brief Muestra si la ruta que se pasó por argumento es un directorio
*/

int es_directorio(char * ruta);

/**
 * @brief Buscara el directorio y contará la cantidad d archivos que hay en el
*/
int buscar(char * dir, char * patron);

int main(int argc, char *argv[]){
    char *dir;
    char *pattern;

    // Terminar temprano
    if(argc != 3){
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    dir = argv[1];
    pattern = argv[2];
    //se crea variable para capturar el valor que retorna de la funcion 'buscar()' y se imprime la cantidad de archivos encontrados 
    int total = buscar(dir, pattern);
    printf("Archivos encontrados: %d\n", total);

    exit(EXIT_SUCCESS);
}

int es_directorio(char* ruta){
  
  //Se declara el struct de tipo stat
  struct stat s;

  /*
    Se le dará un valor entero a resultado para saber si
    la ruta que se pasó por argumento es un directorio existente
  */ 
  int resultado = stat(ruta, &s);

  //Si el resultado anterior es -1, se indicará como error
  if(resultado < 0){
    perror("stat");
    return 0;
  }
  /*
    En el caso contrario, pasará a comparar en que modo está lo encontrado
    y retornará 1 si es un directorio, en el caso que sea un archivo se retornará un 0
  */
  if(S_ISDIR(s.st_mode)){
    return 1;
  }
  else{
    return 0;
  }
}

int buscar(char * dir, char * patron){
  //Se inicializa una variable "total" para conocer cuantos archivos tienen el mismo nombre
  int total = 0;

  //Se abre el directorio dir
  DIR *d = opendir(dir);

  //Se declara el struct ent de tipo dirent
  struct dirent *ent;

  //Se declara una cadena que contendrá/concatenrá la ruta del archivo
  char *ruta;
  
  //Verificará si al intentar abrir el directorio "dir" no se encontró nada y lanzará un error además de retornar el total
  if(d == NULL){
    perror("opendir");
    return total;
  }

  //Se leerá el directorio con readdir
  while((ent = readdir(d))!= NULL){

    //Se le da la suficiente memoria a ruta, para que pueda almacenar toda la ruta del directorio
    ruta = (char*)malloc(strlen(dir) + strlen(ent->d_name)+2);
    ruta[0] = '\0';

    //Se verifica si ruta es nulo para indicar que hubo un error y salir del programa
    if(ruta==NULL){
      perror("malloc");
      exit(EXIT_FAILURE);
    }

    /*
      En el caso de que ruta no sea nula, se le empezarán a concatenar el resto de 
      elementos que se necesitarán
    */
    strcat(ruta, dir);
    strcat(ruta,"/");
    strcat(ruta,ent->d_name);

    /*
      Se verifica si la entrada contiene el patrón que se le pasa por argumento, en el
      caso de que si lo contenga, se imprimirá su ruta.
    */ 
    if(strstr(ent->d_name, patron)!=NULL){
      printf("%s\n",ruta);
      total++;
    }

    /*
      Se verifica si la entrada es un directorio y también que no sea un directorio actual (.)
      o sea un directorio anterior (..) para evitar que termine la función 
    */
    if(es_directorio(ruta) && strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0){
      total += buscar(ruta,patron); 
    }
  }
  free(ruta);
  closedir(d);
  return total;
}

void usage(char * programName){
  printf("Busqueda de archivos\n");
  printf("Uso: %s DIR PATTERN", programName);  
  printf("\nDIR: Directorio base de busqueda" );
  printf("\nPATTERN: texto contenido en el nombre del archivo/directorio" );
  printf("Busca los archivos y/o directorios dentro de DIR cuyo nombre\ncontiene PATTERN\n" );
}