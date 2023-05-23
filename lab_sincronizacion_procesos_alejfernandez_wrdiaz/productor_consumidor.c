#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/*
    @brief Definicion del tipo de semaforo
*/
typedef sem_t semaphore;

//Tamaño del buffer
int n;

/*  @brief Alias para sem_wait (detener el proceso)*/
#define down sem_wait
/*  @brief Alias para sem_post ()*/
#define up sem_post

int finished = 0;

semaphore mutex; //Acceso exclusivo al Buffer
semaphore empty; //Productor
semaphore full; //Consumidor

/*
*   @brief Hilo Productor
*   @param Argumento al hilo (no usado)
*   @return Valor de retorno (no usado)
*/
void * producer(void * arg);

/*
*   @brief Hilo Consumidor
*   @param Argumento al hilo (no usado)
*   @return Valor de retorno (no usado)
*/
void * consumer(void * arg);

/*
*   @brief Produce un nuevo item
*   @return Nuevo item 
*/
int produce_item();
/*
*   @brief Inserta un nuevo item
*   @param item Item a agregar 
*/
void insert_item(int item);

/*
*   @brief Remueve un item 
*   @return Item removido
*/
int remove_item();

/*
*   @brief Consume un item
*   @param item Item a consumir 
*/
void consume_item(int item);

int main(int argc, char * argv[]){

    n = 0;

    if(argc == 2){
        n = atoi(argv[1]);
    }

    while(n < 2){
        printf("Ingrese la cantidad para el buffer: ");
        scanf("%d", &n);
    }

    pthread_t t_prod, t_cons;
    
    //Inicializar los semaforos
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, n);
    sem_init(&full, 0, 0);

    //Crear el hilo productor
    pthread_create(&t_prod, NULL, producer, NULL);
    //Crear el hilo consumidor
    pthread_create(&t_cons, NULL, consumer, NULL);

    sleep(1);

    finished = 1;

    pthread_join(t_prod, NULL);
    pthread_join(t_cons, NULL);

    exit(EXIT_SUCCESS);
}

void * producer(void * arg) {
    int item;
    while (!finished) {
        item = produce_item(); //Producir un nuevo item
        down(&empty); //Bloquear en el semaforo empty
        down(&mutex); //Inicio de la seccion crítica para insertar
        insert_item(item); //Inserta el item producido en el buffer
        up(&mutex); //Fin de la seccion crítica para insertar
        up(&full); //Desbloquear el semaforo full
    }
}
void * consumer(void * arg) {
    int item;
    while (!finished) {
        down(&full); //Bloquear el semaforo full
        down(&mutex); //Inicio de la sección crítica para sacar el item 
        remove_item(item); //Remueve el item en el buffer
        up(&mutex); // Fin de la sección crítica
        up(&empty); //Desbloquear el semaforo empty
        consume_item(item); //Consume el item
    }
}

int produce_item(){
    printf(" P ");
    fflush(stdout);
    usleep(rand()%1000);
    return 0;
}

void insert_item(int item){
    printf(" I ");
    fflush(stdout);
}

int remove_item(){
    printf(" R ");
    fflush(stdout);
    return -1;
}

void consume_item(int item){
    printf(" C ");
    fflush(stdout);
}
