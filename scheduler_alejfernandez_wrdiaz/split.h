/*
 * String split library
 * Author Erwin Meza Vega <emezav@gmail.com>
*/

#ifndef SPLIT_H_
#define SPLIT_H_

#define MAX_PARTS 255

/**
 * @brief sLista de subcadenas
 */
typedef struct {
    char * parts[MAX_PARTS]; /*!< Arreglo de subcadenas */
    int count; /*!< Cantidad de subcadenas*/
}split_list;

/**
 * @brief Divide una cadena en partes.
 * @param str Cadena a dividir
 * @param delim Delimitadores, o NULL para delimitadores por defecto.
 */
split_list * split(char * str, char * delim);


#endif
