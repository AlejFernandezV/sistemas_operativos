#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    char * message = "Bye bye";
    char * message2 = "Bye bye!";

    write(2, message, sizeof(char)*strlen(message));
    fwrite(message2, sizeof(char), strlen(message), stderr);
    
    exit(EXIT_SUCCESS);
}