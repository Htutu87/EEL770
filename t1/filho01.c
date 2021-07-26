/*
EEL770 - Sistemas Operacionais
UFRJ | 2021.1 | Prof.: Rodrigo Couto
Autor: Artur Amaral | DRE: 119057968
Descrição: Código fonte do processo filho da tarefa um.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define OK          0
#define MIN_RAND    1
#define MAX_RAND    100
#define MAX_BUFFER  sizeof(int) 

int main(){

    time_t t;

    srand((unsigned) time(&t));

    int numeroSorteado = rand() % (MAX_RAND - MIN_RAND + 1) + MIN_RAND;

    printf("------------------------------------\n");
    printf("\n Executando tarefa 01 (pid: %u).\n", getpid());
    printf("\n\tNumero aleatorio sorteado: %i.\n", numeroSorteado);
    printf("\n------------------------------------\n");

    return OK;
}
