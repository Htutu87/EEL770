#ifndef BABUINOS_H
#define BABUINOS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define NORTE_SUL 0
#define SUL_NORTE 1
#define NUM_INICIAL_NORTE 1
#define NUM_INICIAL_SUL 1

typedef struct
{
    unsigned int numBabuinosNorte,
                numBabuinosSul,
                numBabuinosAtravessando;
    const unsigned int *capacidade;
    bool direcaoAtual, livre;
} corda_t;


void printaCorda(corda_t *corda)
{
   printf("-> numBabuinosNorte: %u\n", corda->numBabuinosNorte); 
   printf("-> numBanuinosSul: %u\n", corda->numBabuinosSul); 
   printf("-> numBabuinosAtravessando: %u\n", corda->numBabuinosAtravessando); 
   printf("-> direcaoAtual: "); 
   corda->direcaoAtual ? printf("SUL->NORTE\n") : printf("NORTE->SUL\n") ; 
   printf("-> livre: ");
   corda->livre ? printf("Sim\n") : printf("Nao\n"); 
}

//--------------------------------------------------------------------------
// Função auxiliar que fornece temporização para o programa.

void delay_seconds(double segundos)
{ 
    clock_t tempo_inicial;
    double tempo_decorrido = 0;

    tempo_inicial = clock();

    while (tempo_decorrido <= segundos)
        tempo_decorrido = (double)(clock() - tempo_inicial) / CLOCKS_PER_SEC;
}

#endif
