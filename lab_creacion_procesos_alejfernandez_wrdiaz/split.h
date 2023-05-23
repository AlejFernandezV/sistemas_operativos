/*

 * @brief split library
 * @Author Erwin Meza Vega <emezav@gmail.com> 
*/

#ifndef SPLIT_H_
#define SPLIT_H_

#define MAX_PARTS 255

/**
 * @brief Almacena la lista de partes de una cadena 
 * Define la estructura de la lista de palabras obtenida como valor de retorno
 * de la funci�n split.
 */
typedef struct {
    char * parts[MAX_PARTS]; /*!< Partes obtenidas*/
    int count; /*!< Conteo de partes*/
}split_list;

/** 
 * @brief Divide una cadena por los delimitadores especificados
 * @param str Cadena a dividir
 * @param delim Cadena que contiene los delimitadores.
 * @return Estructura de datos con las partes de la linea
 * Divide una cadena en palabras, usando los delimitadores especificados
 * o los delimitadores por defecto
 */
split_list * split(char * str, const char * delim);

#endif
