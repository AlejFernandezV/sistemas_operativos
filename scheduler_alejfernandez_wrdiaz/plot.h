/**
 * @file
 * @brief Definiciones del modulo para generar el diagrama de Gantt de la planificacion.
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
*/


#include <stdio.h>
#include <stdlib.h>

#include "list.h"


/**
 * @brief Crea el diagrama de Gantt con los procesos simulados.
 * @param path Ruta del archivo de salida
 * @param processes Lista de procesos simulados.
 * @return 1 si se puede crear la grafica, 0 en caso contrario.
*/
int create_plot(char * path, list* processes);
