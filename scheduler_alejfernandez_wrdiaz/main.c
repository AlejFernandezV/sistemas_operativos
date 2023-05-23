/**
 * @file
 * @brief Simulador de planificacion de CPU para un solo procesador.
 * @author  Erwin Meza Vega <emezav@gmail.com>.
 * @note El simulador require el ejecutable gnuplot en el PATH.
 * Windows: https://sourceforge.net/projects/gnuplot/files/gnuplot/5.4.6/gp546-win64-mingw-2.exe/download
 * Linux: apt install gnuplot
 *
 * Ejecucion:
 *  ./main archivo_configuracion
 *  ./main < archivo_configuracion
 * Ejemplos:
 *  ./main test/fifo.txt
 *  ./main rr.txt
 *  ./main sjf.txt
 *  ./main srt.txt
 * Documentacion:
 *  make doc
 * Limpiar:
 *  make clean
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "plot.h"
#include "sched.h"
#include "split.h"
#include "util.h"

/** @brief Nombre por defecto de la grafica de simulacion. */
#define DEFAULT_FILENAME "gantt"

/**
 * @brief Compara dos procesos por su nombre
 * @param a Proceso 1
 * @param b Proceso 2
 * @return strcmp(p2->name, p1->name)
*/
int compare_process_name(const void * a, const void * b);

/**
 * @brief Programa principal
 * @param argc Cantidad de argumentos de linea de comandos
 * @param argv Arreglo de cadenas con los argumentos de linea de comandos
 */
int main(int argc, char * argv[]) {
  char linea[80];
  FILE *fd;
  split_list * t;
  char ** args;
  int arrival_time;
  int execution_time;
  int priority;
  int quantum;
  int simulated = 0;
  int current_pid;

  int i;
  int nqueues;

  /* Referencia al arreglo de colas de prioridad */
  priority_queue * queues;

  /* Lista de procesos */
  list * processes;

  /* Referencia a un proceso */
  process * p;

  /* Nombre de archivo con los parametros de simulacion, si es proporcionado por linea de comandos */
  char * filename;

  /* Nombre del archivo en el cual se guarda el diagrama de Gantt */
  char * plot_filename;

  /* Leer el archivo de configuracion, pasado como parametro al main
     o por redireccion de la entrada estandar */

  //Asignar un nombre por defecto al archivo
  filename = "gantt";


  if (argc < 2) {
    /* Si el programa se invoca sin argumentos, tomar la entrada de Stdin. */
    fd = stdin;
  }else {
    /* Si se especifica el nombre de un archivo con los parametros de la simulacion, tomar la entrada del archivo. */
    filename = argv[1];
    fd = fopen(filename, "r");
    if (!fd) {
      fd = stdin;
      filename = "gantt";
      }
  }

  //Nombre del archivo de la grafica
  plot_filename = concat(remove_extension(filename), ".gpi");

  nqueues = 0;

  /* Lectura del archivo de configuracion */
  while (!feof(fd)) {

    //Leer una linea del flujo de datos de entrada
    memset(linea, 0, 80);
    fgets(linea, 80, fd);

    //printf("Linea leida: %s", linea);

    if (strlen(linea) <= 1) {continue;}
    if (linea[0] == '#') {continue;}

    //Convertir en minusculas
    lcase(linea);

    // Partir la linea en tokens
    t = split(linea, 0);

    //Ignora las lineas que no contienen tokens
    if (t->count == 0) { continue;}

    //Procesar cada linea, args apunta al arreglo de tokens
    args = t->parts;

    if (equals(args[0], "define") && t->count >= 3) {
      //Comando define queues n
      if (equals(args[1], "queues")) {
        nqueues = atoi(args[2]);
        //Crear el arreglo de colas de prioridad y a lista de procesos
        if (nqueues > 0) {
          queues = create_queues(nqueues);
          processes = create_list();
        }
        simulated = 0;
        current_pid = 0;
      }else if (equals(args[1], "scheduling") && t->count >= 4){
        //Comando scheduling n ESTRATEGIA
        //n = 1 ... # de colas de prioridad

        //i = n - 1, los arreglos comienzan en cero
        i = atoi(args[2]) - 1;
        //printf("Defining scheduling to queue %d\n", i);
        if (i < 0 || i >= nqueues) {continue;}
        if (equals(args[3], "rr")) {
          queues[i].strategy = RR;
        }else if (equals(args[3], "sjf")) {
          queues[i].strategy = SJF;
        }
        else if (equals(args[3], "fifo")) {
          queues[i].strategy = FIFO;
        }
        else if (equals(args[3], "srt")) {
          queues[i].strategy = SRT;
        }
      }else if (equals(args[1], "quantum") && t->count >= 4){
        //Comando scheduling n QUANTUM
        //n = 1 ... # de colas de prioridad
        i = atoi(args[2]) - 1;
        //printf("Defining quantum to queue %d\n", i);
        quantum = atoi(args[3]);
        if (i < 0 || i >= nqueues) {continue;}
        queues[i].quantum = quantum;
      }
    }
    else if (equals(args[0], "process") && t->count >= 5){
      //Comando process NAME ARRIVAL_TIME EXEC_TIME PRIORITY
      //printf("process %s\n", args[1]);

      //Verificar el tiempo de llegada
      arrival_time = -1;
      arrival_time = atoi(args[2]);
      if (execution_time < 0) {
          fprintf(stderr, "Arrival time must be greater than zero\n");
          continue;
      }

      //Verificar el tiempo de ejecucion
      execution_time = -1;
      execution_time = atoi(args[3]);
      if (execution_time <= 0) {
          fprintf(stderr, "Execution time must be greater than zero\n");
          continue;
      }

      //Verificar la cola de prioridad
      priority = -1;
      priority = atoi(args[4]);
      if (priority <= 0 || priority > nqueues) {
          fprintf(stderr, "Priority %d does not exist\n", priority);
          continue;
      }

      p = create_process(args[1], arrival_time, execution_time);

      p->priority = priority - 1;
      p->pid = ++current_pid;

      //Insertar el proceso en la lista general de procesos
      insert_ordered(processes, p, compare_process_name);
    }
    else if (equals(args[0], "start")) {
      //Comando start
      //Comenzar la simulacion!!!
      schedule(processes, queues, nqueues);

      //Crear la grafica de la simulacion
      create_plot(plot_filename, processes);

      printf("Gannt plot saved to %s\n", plot_filename);

      simulated = 1;

    }else if (equals(args[0], "exit")) {
      printf("Finished\n");
      exit(EXIT_SUCCESS);
    }
  }
}

int compare_process_name(const void * a, const void * b) {
  process * p1;
  process * p2;

  p1 = (process*)a;
  p2 = (process*)b;

  return strcmp(p2->name, p1->name);
}
