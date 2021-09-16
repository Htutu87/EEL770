/*  Pseudo código
 *  
 *  #travessia Norte-Sul
 *
 *  -> Babuino checa se há alguem vindo em direcao contraria 
 *  (checa variável de condição!!! Recurso compartilhado entre as threads).
 *  -> Se tiver, o babuino coloca a thread pra dormir(dentro do while, ainda
 *  checando pela variável.)
 *  -> Se não tiver, ele indica que vai atravessar na direção norte sul e
 *   atravessa de fato.
 *   -> Ao chegar do outro lado, ele sinaliza que chegou. 
 *
 * */

// Talvez eu tenha que criar duas filas de espera, uma para o norte, outra para
// o sul, pois o signal pode acabar acordando um thread do mesmo lado. Talvez não.

// Uma lock só pode ser adquirido e readquirido uma única vez dentro da thread.
// Para quebrar essa condição, deve-se utiliza um mutex_recursivo.


#include "babuinos.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockNorte = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockSul = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t filaSul = PTHREAD_COND_INITIALIZER;
pthread_cond_t filaNorte = PTHREAD_COND_INITIALIZER;

void *travessiaNorteSul(void *arg)
{
    corda_t *corda = (corda_t *) arg; 
    unsigned short babuinoId = 0 + rand() % ( (100 + 1) - 0);

    pthread_mutex_lock(&lock);

    while(corda->direcaoAtual == SUL_NORTE) 
        pthread_cond_wait(&filaNorte, &lock);
    
    if (corda->numBabuinosAtravessando > *corda->capacidade)
    {
        printf("[N%u] Corda lotada. Babuino tentara novamente mais tarde.\n",
                babuinoId);
        pthread_mutex_unlock(&lockNorte);
    }
    else
    {
        corda->numBabuinosAtravessando++;
        
        pthread_cond_signal(&filaNorte);
        
        for (int i = 1; i <= 5 ; i++)
        {
            printf("[N%u] Percorreu: %u%%\n", babuinoId, i*20);
            //for (int j = 1; j <= i; j++) printf("X");
            //printf("]\n");
            delay_seconds(1);
        } 

        pthread_mutex_lock(&lock);

        corda->numBabuinosAtravessando--;
        corda->numBabuinosNorte--; 
        
        if (corda->numBabuinosAtravessando == 0)
        {
            corda->direcaoAtual = SUL_NORTE;
            pthread_cond_signal(&filaSul);
        }

        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

void *travessiaSulNorte(void *arg)
{
    corda_t *corda = (corda_t *) arg; 
    unsigned short babuinoId = 0 + rand() % ( (100 + 1) - 0);

    pthread_mutex_lock(&lock);

    while(corda->direcaoAtual == NORTE_SUL) 
        pthread_cond_wait(&filaSul, &lock);
    
    if (corda->numBabuinosAtravessando > *corda->capacidade)
    {
        printf("[S%u] Corda lotada.\n", babuinoId);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        corda->numBabuinosAtravessando++;
        
        pthread_cond_signal(&filaSul);
         
        for (int i = 1; i <= 5; i++)
        {
            printf("[S%u] Percorreu: %u%%\n", babuinoId, i*20);
            //for (int j = 1; j <= i; j++) printf("X");
            //printf("]\n");
            delay_seconds(1);
        } 

        pthread_mutex_lock(&lock);

        corda->numBabuinosAtravessando--;
        corda->numBabuinosSul--;

        if (corda->numBabuinosAtravessando == 0)
        {
            corda->direcaoAtual = NORTE_SUL;
            pthread_cond_signal(&filaNorte);
        }

        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

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

    while(corda.numBabuinosNorte != 0 && corda.numBabuinosSul != 0)
    { 
        pthread_create(&ns, NULL, travessiaNorteSul, &corda);
        pthread_create(&sn, NULL, travessiaSulNorte, &corda);

        // OBS: O pthread join faz a thread executar ininterruptamente até
        // o final, que não é o que eu quero. Porém, seu uso faz com que os
        // babuinos não emperrem na segunda aquisição do lock. Investigar melhor
        // isso.
        //
        //pthread_join(ns, NULL);
        //pthread_join(sn, NULL); 
    }

    return 0;
}
