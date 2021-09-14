
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define NORTE_SUL 0
#define SUL_NORTE 1
#define NUM_INICIAL_NORTE 20
#define NUM_INICIAL_SUL 20

//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
//                 void *(*start_routine)(void*), void *arg);

//--------------------------------------------------------------------------
// Estruturas auxiliares

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

//--------------------------------------------------------------------------
//
// Funções de travessia dos babuínos.
// Cada função de travessia representa uma thread, que deve ser executada
// ininterruptamente até seu fim, para o funcionamento correto do programa.

void *travessiaNorteSul(void *arg)
{
 
    corda_t *corda = (corda_t *) arg; 
    /* Babuino verifica se existem condições para ele atravessar.
     *
     * Se a direcao atual for sul-norte significa que o último babuino
     * a atravessar veio do sul, logo, o estado ficou salvo como sul_norte.
     * Mas se a corda estiver livre, significa que, mesmo a direcao atual
     * sendo contrária, o babuino pode atravessar, invertendo a atual direcao
     * da corda. Caso a direcao atual seja igual ao objetivo deste babuino,
     * este if é ignorado e o procedimento segue normalmente. */
    if (corda->numBabuinosNorte == 0)
    {
        printf("Nao ha babuinos para atravessar.\n");
        return NULL;
    }

    unsigned int babuinoId = 0 + rand() % (( 100 + 1 ) - 0);

    //LOCK
    if (corda->direcaoAtual == SUL_NORTE)
        if (corda->livre)
        {
            corda->direcaoAtual = !corda->direcaoAtual;
            corda->livre = !corda->livre;
        }
    //UNLOCK

    // while corda->babuinosAtravessando >= *(corda->capacidade)
    // ESPERAR CAPACIDADE BAIXAR. Aqui, usar-se-á uma variável de condição.

    /* Travessia do babuino */

    corda->numBabuinosNorte--;
    corda->numBabuinosAtravessando++;

    for (int i = 1; i <= 5; i++)
    {
        printf("Babuino [N%u] atravessou %u%% da corda\n", 
                babuinoId,
                i*20);
        delay_seconds(1);
        //printaCorda(corda);
    }
    corda->numBabuinosAtravessando--;
    corda->numBabuinosSul++;
    return NULL;
}

void *travessiaSulNorte(void *arg)
{
    corda_t *corda = (corda_t *) arg; 

    if (corda->numBabuinosSul == 0)
    {
        printf("Nao ha babuinos para atravessar.\n");
        return NULL;
    }
 
    unsigned int babuinoId = 0 + rand() % (( 100 + 1 ) - 0);

    //LOCK
    if (corda->direcaoAtual == NORTE_SUL)
        if (corda->livre)
        {
            corda->direcaoAtual = !corda->direcaoAtual;
            corda->livre = !corda->livre;
        }
    //UNLOCK

    // while corda->babuinosAtravessando >= *(corda->capacidade)
    // ESPERAR CAPACIDADE BAIXAR. Aqui, usar-se-á uma variável de condição.

    /* Travessia do babuino */

    corda->numBabuinosSul--;
    corda->numBabuinosAtravessando++;

    for (int i = 1; i <= 5; i++)
    {
        printf("Babuino [S%u] atravessou %u%% da corda\n", 
                babuinoId,
                i*20);
        delay_seconds(1);
        //printaCorda(corda);
    }
    corda->numBabuinosAtravessando--;
    corda->numBabuinosNorte++;
    return NULL;
}

//--------------------------------------------------------------------------
// Função principal

int main()
{

    pthread_t ns;
    pthread_t sn;

    const unsigned int capacidade = 5;

    corda_t corda = {.numBabuinosNorte = NUM_INICIAL_NORTE,
             .numBabuinosSul = NUM_INICIAL_SUL,
             .numBabuinosAtravessando = 0,
             .capacidade = &capacidade,
             .livre = true,
             .direcaoAtual = NORTE_SUL};

    srand(time(NULL));

    pthread_create(&ns, NULL, travessiaNorteSul, &corda);
    pthread_create(&sn, NULL, travessiaSulNorte, &corda);

    printaCorda(&corda);
    pthread_join(sn, NULL);
    pthread_join(ns, NULL);
    printaCorda(&corda);

    return 0;
}
