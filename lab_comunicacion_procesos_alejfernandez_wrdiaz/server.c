#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

/*
*   @brief
*   @param
*/
void handle_signal(int sig);

/*
*   @brief
*   @param
*/
void terminate();

/*
*   @brief controla la terminacion del programa
*/
int finished = 0;

int main(int argc, char * argv[]){
    //Estructura para el manejador de la señal
    struct sigaction act;
    
    //Socket del servidor
    int s;

    //Socket del cliente
    int c;

    //Direccion IPv4 del servidor
    struct sockaddr_in addr;

    //Direccion IPv4 del cliente
    struct sockaddr_in c_addr;
    socklen_t c_len = 0;

    memset(&act, 0, sizeof(struct sigaction));

    act.sa_handler = handle_signal;

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    printf("Server started...\n");

    //1. Crear el conector: remoto, IPv4, flujo
    s = socket(AF_INET,SOCK_STREAM, 0);

    //2. Asociar el socket csignalon una direccion IPv4
    //2.1 Configurar la direccion IPv4
    //Rellenar la estructura con ceros (NULL)
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    //El servidor escucha en cualquier servicio de red
    inet_aton("0.0.0.0", &addr.sin_addr);

    bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    //3. Poner el socket disponible (escuchar)
    listen(s,10);

    //4. Esperar por una conexion

    c = accept(s,(struct sockaddr *)&c_addr, &c_len);

    while(!finished){

    }

    //5. Cerrar la conexion con el cliente
    close(c);

    //
    close(s);

    printf("Servidor apago...\n");

    exit(EXIT_SUCCESS);
}

void handle_signal(int sig){
    printf("Signal %d receive\n", sig);
    finished = 1;
}