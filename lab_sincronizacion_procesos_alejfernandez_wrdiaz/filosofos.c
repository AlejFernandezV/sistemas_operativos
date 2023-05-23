#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define LEFT ((i-1+n)%n)
#define RIGHT  ((i+1)%n)
/*
    @brief Definicion del tipo de semaforo
*/
typedef sem_t semaphore;

/*
*   @brief Enumeracion que define el estado del filósofo
*/
enum philosopher_state{HUNGRY,EATING,THINKING};

/*
*   @brief Posición izquierda del filosofo i 
*/
int left;

/*
*   @brief Posición derecha del filosofo i 
*/
int right;

/*
*   @brief Posicion i de los filosofos
*/
int i;

/*  @brief Alias para sem_wait (detener el proceso)*/
#define down sem_wait
/*  @brief Alias para sem_post ()*/
#define up sem_post

semaphore mutex;//Acceso exclusivo al Buffer

/*
*  @brief cantidad de filosofos
*/
int n ;

/*  
*   @brief Bandera que indica la finalizacion del bucle while
*/
int finished;

/*
*   @brief Arreglo de los estados de los filosofos
*/
int * state;

/*
*   @brief Arreglo de los identificadores de los filosofos
*/
int * ids;

/*
*   @brief Arreglo de semaforos
*/
semaphore * s; 

/*
*   @brief Hilo de filosofo
*   @param i posicion del filosofo
*   @return Valor de retorno del hilo (no usado)
*/
void * philosopher(void * arg);

/*
*   @brief 
*   @param i posicion del filosofo
*   @return Valor de retorno del hilo (no usado)
*/
void take_forks(int i);

/*
*   @brief Hilo de filosofo
*   @param i posicion del filosofo
*   @return Valor de retorno del hilo (no usado)
*/
void put_forks(int i);

/*
*   @brief Verifica el estado del filosofo i, del LEFT y del RIGHT
*   @param i posicion del filosofo
*   @return Valor de retorno del hilo (no usado)
*/
void test(int i);

/*
*   @brief Accion donde esta pensando el filosofo
*/
void think(int i);

/*
*   @brief Accion donde esta comiendo el filosofo
*/
void eat(int i);


int main(int argc, char * argv[]){

    //Arreglo de hilos
    pthread_t * threads;

    //Validar la cantidad de filosofos
    n = 0;

    if(argc == 2){
        n = atoi(argv[1]);
    }

    while(n <= 2){
        printf("Ingrese la cantidad de filosofos: ");
        scanf("%d", &n);
    }

    //Reservar memoria para el arreglo de identificadores
    ids = (int *)malloc(n *sizeof(int));
    //Reservar memoria para el arreglo de filosofos
    s = (semaphore *)malloc(n * sizeof(semaphore));
    //Reservar memoria para el arreglo de estados
    state = (int *)malloc(n * sizeof(int));
    //Reservar memoria para el arreglo de hilos
    threads = (pthread_t *)malloc(n * sizeof(pthread_t));
    
    //Inicializar el mutex
    sem_init(&mutex, 0, 1);

    //Inicializar los semaforos en 0 y 
    //asignarle en su respectiva posicion 
    //el identificador
    for(i = 0; i < n; i++){
        ids[i] = i;
        printf("%d \n",i);
        pthread_create(&threads[i], NULL, philosopher, (void *)&ids[i]);
    }

    usleep(1);

    finished = 1;

    for(i=0; i < n; i++){
        printf("ELIMINANDO HILO %d \n",i+1);
        pthread_join(threads[i],NULL);
    }

    sem_destroy(&mutex);
    exit(EXIT_SUCCESS);
}

void * philosopher(void * arg) {
    int i = * (int *) arg;
    while (!finished) {
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
    }
}
void take_forks(int i) {
    down(&mutex);
    state[i]=HUNGRY;
    test(i);
    up(&mutex);
    down(&s[i]); 
}
void put_forks(int i) {
    down(&mutex);
    state[i]=THINKING;
    test(LEFT);
    test(RIGHT);
    up(&mutex);
}
void test(int i) {
    if (state[i] == HUNGRY
        && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i]=EATING;
        up(&s[i]); 
    }
}

void think(int i){
    printf("Filosofo %d: Pensando \n",i+1);
    usleep(1);
    fflush(stdout);
}

void eat(int i){
    printf("Filosofo %d: Comiendo \n",i+1);
    usleep(1);
    fflush(stdout);
}