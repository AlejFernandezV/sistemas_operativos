/**
 * @file
 * @brief Funciones de listas enlazadas
 * @author  Erwin Meza Vega <emezav@gmail.com>.
*/


#include <stdlib.h>
#include "list.h"

list * create_list() {
       list *ret;

       ret = (list *) malloc(sizeof(list));

       ret->head = 0;
       ret->tail = 0;
       ret->count = 0;
       return ret;
}

node *
       create_node(void * data) {
         node * ret;

         ret = (node *)malloc(sizeof(node));
         ret->data = data;
         ret->next = 0;
         ret->previous = 0;

         return ret;
}

void clear_list(list *l, int erase_data) {
     if (l ==0) {return;}
     node *aux, *ptr;

     ptr = l->head;

     //printf("Clearing list...\n");
     while (ptr != 0) {
           aux = ptr;
           ptr = ptr->next;
           if (erase_data) {
            free(aux->data);
           }
           free(aux);
           aux = NULL;
     }

     //printf("Finished clearing list\n");

     l->head = 0;
     l->tail = 0;
     l->count = 0;
}

void destroy_list(list * l, int erase_data) {
   clear_list(l, erase_data);
   free(l);
}

list * insert_ordered(list *l, void * data,
                                int (*compare)  (const void *, const void *)) {

    node * n;
    node * aux, *ant;
    if (l ==0) { return 0;}
    n = create_node(data);

    if (l->head == 0) { /*Primer elemento en la lista  */
          l->head = n;
          l->tail = n;
          l->count = 0;
    }else {
          ant = 0;
          aux = l->head;
          while (aux != 0 && compare(data, aux->data) <= 0) {
              ant = aux;
              aux = aux->next;
          }

          n->previous = ant;

          if (ant == 0) { //Primer nodo de la lista
             n->next = l->head;
             l->head->previous = n;
             l->head = n;
          }else {
              ant->next = n;
              n->next = aux;
              if (aux == 0) { //Final de la lista
                 l->tail = n;
              }else {
                    aux->previous = n;
              }
          }

    }

    l->count++;

    return l;
}

list * push_front(list *l, void * data) {
       node * n;

       if (l ==0) {return 0;}

       n = create_node(data);

       if (l->head == 0) { /*Primer elemento en la lista  */
          l->head = n;
          l->tail = n;
          l->count = 0;
       }else {
          n->next = l->head;
          l->head->previous = n;
          l->head = n;
       }

       l->count++;

       return l;
}

list * push_back(list *l, void * data) {
       node * n;

       if (l ==0) {return 0;}

       n = create_node(data);

       if (l->tail == 0) { /*Primer elemento en la lista */
          l->head = n;
          l->tail = n;
          l->count = 0;
       }else {
          n->previous = l->tail;
          l->tail->next = n;
          l->tail = n;
       }
       l->count++;

       return l;
}


void * front(const list *l) {
        if (l == 0) {return 0;}
        if (l->head == 0) {return 0;}
        return l->head->data;
}

 void * back(list *l) {
       if (l == 0) {return 0;}
       if (l->tail == 0) {return 0;}
       return l->tail->data;
}

list * pop_front(list *l) {
       node *ret;
       void * data;

      if (l ==0 || l->count == 0) {return l;}

       ret = l->head;

       l->head = l->head->next;

       if (l->head == 0) {
          l->tail = 0;
       }else {
          l->head->previous = 0;
       }

       l->count--;

       data =  ret->data;

       free(ret);

       return l;
}

list *  pop_back(list *l) {
       node *ret;
       void * data;

       if (l ==0 || l->count == 0) {return l;}

       ret = l->tail;

       l->tail = l->tail->previous;

       if (l->tail == 0) { /* Un solo elemento? */
          l->head = 0;
       }else {
          l->tail->next = 0;
       }

       l->count--;

       data =  ret->data;

       free(ret);

       return l;
}

int empty(list *l) {
       return ((l== 0) || l->head==0);
}

node_iterator head(list *l) {
       if (l ==0) {return 0;}
       return l->head;
}

 node_iterator tail(list *l) {
       if (l ==0) {return 0;}
       return l->tail;
}


node_iterator next(node_iterator it) {
       if (it == 0) {return 0;}
       return it->next;
}

node_iterator previous(node_iterator it) {
       if (it == 0) {return 0;}
       return it->previous;
}