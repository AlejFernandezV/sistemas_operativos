/**
 * @file
 * @brief Funciones de listas enlazadas
 * @author  Erwin Meza Vega <emezav@gmail.com>.
*/

#ifndef _LIST_H
#define _LIST_H


/**
 * @brief Nodo de lista
*/
typedef struct node {
       void * data; /*!< Apundator al dato */
       struct node *next;  /*!< Apuntador al siguiente nodo */
       struct node *previous; /*!< Apuntador al nodo anterior*/
}node;

/** @brief Iterador de nodos. */
typedef node * node_iterator;

/**
 * @brief Lista doble enlazada
*/
typedef struct {
       node *head; /*!< Apuntador a la cabeza de la lista */
       node *tail; /*!< Apuntador a la cola de la lista */
       int count; /*!< Cantidad de elementos en la lista */
} list;

/**
 * @brief Crea una nueva lista vacia.
 * @return Nueva lista vacia.
*/
list * create_list();

/**
 * @brief Crea un nuevo nodo de lista.
 * @param data Apuntador al dato almacenado dentro del nodo
 * @return Nuevo nodo
*/
node * create_node(void * data);

/**
 * @brief Elimina los nodos de la lista
 * @param l Lista a limpiar
 * @param erase_data 1 = libera la memoria del nodo y el dato, 0, solo libera el nodo.
*/
void clear_list(list *l, int erase_data);

/**
 * @brief Libera la memoria asignada a una lista y sus nodos
 * @param l Lista
 * @param erase_data Libera tambien los datos
*/
void destroy_list(list *l, int erase_data);

/**
 * @brief Insertar un nuevo nodo en una lista de acuerdo con la funcion de ordenamiento
 * @param l Lista que recibe el nuevo dato
 * @param data Dato a almacenar dentro de un nuevo nodo de la lista
 * @param compare Funcion de comparacion similar a strcmp
 * @return Apuntador a la lista modificada.
*/
list * insert_ordered(list *l, void * data,
                                int (*compare)  (const void *, const void *));

/**
 * @brief Inserta un nuevo dato al inicio de la lista
 * @param l Lista que recibe el nuevo dato
 * @param data Apuntador al nuevo dato
 * @return Lista modificada
*/
list * push_front(list *l, void * data);

/**
 * @brief Inserta un nuevo dato al final de la lista
 * @param l Lista que recibe el nuevo dato
 * @param data Apuntador al nuevo dato
 * @return Lista modificada
*/
list * push_back(list *l, void * data);


/**
 * @brief Quita el nodo al inicio de la lista
 * @param l Lista de la cual se saca el nodo
 * @return Lista modificada
*/
list * pop_front(list *l);

/**
 * @brief Quita el nodo al final de la lista
 * @param l Lista de la cual se saca el nodo
 * @return Lista modificada
*/
list *  pop_back(list *l);

/**
 * @brief Obtiene el dato que se encuentra al inicio de la lista
 * @param l Lista de la cual se va a obtener el dato
 * @return Dato dentro del primer nodo de la lista
*/
void * front(const list *l) ;

/**
 * @brief Obtiene el dato que se encuentra al final de la lista
 * @param l Lista de la cual se va a obtener el dato
 * @return Dato dentro del ultimo nodo de la lista
*/
void * back(list *l);

/**
 * @brief Verifica si una lista se encuentra vacia
 * @param l Lista a verificar
 * @return 1 = lista vacia, 0 en caso contrario
*/
int empty(list *l);

/**
 * @brief Obtiene un iterador al primer nodo de la lista
 * @param l Lista de la cual se va a obtener el iterador
 * @return Primer nodo de la lista
*/
node_iterator head(list *l);

/**
 * @brief Obtiene un iterador al ultimo nodo de la lista
 * @param l Lista de la cual se va a obtener el iterador
 * @return Ultimo nodo de la lista
*/
node_iterator tail(list *l);

/**
 * @brief Obtiene el siguiente nodo a partir del iterador (nodo) actual
 * @param it Iterador (nodo) actual
 * @return Apuntador al siguiente nodo
*/
node_iterator next(node_iterator it);

/**
 * @brief Obtiene el anterior nodo a partir del iterador (nodo) actual
 * @param it Iterador (nodo) actual
 * @return Apuntador al anterior nodo
*/
node_iterator previous(node_iterator it);

#endif
