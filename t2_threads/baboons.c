
#include "baboons.h"

//---------------------------------------------------------------------------
// Variáveis pertinentes à manipulação das threads.
//
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitNorth = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitSouth = PTHREAD_COND_INITIALIZER;

//---------------------------------------------------------------------------
// Funções "terminais" da travessia dos babuínos.
// São chamadas ao início e ao fim de cada thread se travessia de um babuíno.
//

void StartNorth(rope_t * rope)
{
    static unsigned int consecutive_baboons = 0; 

    // Início do lock -> Como atributos da corda serão manipulados nesta função,
    // deve-se fornecer exclusão mútua entre as threads, para que os dados sejam
    // modificados de forma consistente.
    //
    pthread_mutex_lock(&lock);

    // Se houverem babuínos vindo da direção norte (que é a direção contrária) ou
    // se houverem mais babuínos indo para o sul do que a corda aguenta (5), o 
    // babuíno entra na fila de espera do norte. A sinzalização para que as threads
    // da fila acordem é dada quando um babuíno termina sua travessia (função
    // EndNorth, por exemplo).
    //
    while(rope->baboons_going_north > 0 ||
            rope->baboons_going_south >= *rope->capacity)
        pthread_cond_wait(&waitNorth, &lock);
    
    // Trecho anti-starvation: A variável estática consecutive_baboons conta 
    // quantos babuínos atravessaram nesta direção em sequência. Arbitrariamente,
    // quando 20 babuínos atravessam consecutivamente numa determinada direção,
    // eles sinalizam para os babuínos da direção oposta para que acordem,
    // bem como colocam-se para dormir. Analogamente, eles serão acordados quando
    // os babuínos da outra direção mandarem 20 dos seus consecutivamente.
    //
    if (consecutive_baboons > 20)
    {
        pthread_cond_signal(&waitSouth);
        pthread_cond_wait(&waitNorth, &lock);
        consecutive_baboons = 0;
    }

    // Ao sair do "sono", esta thread acorda mais uma consigo, da sua mesma direção.
    // Caso não haja esta sinalização, os babuínos esperam que um deles chegue
    // ao fim da travessia, para que somente aí sejam acordados, resultando na
    // travessia de um babuíno por vez, que seria um mal aproveitamente de tempo.
    //
    pthread_cond_signal(&waitNorth);

    // Alteração do estado da corda, e sinalização da quantidade de babuínos
    // consecutivos.
    //
    rope->baboons_going_south++;
    consecutive_baboons++;

    printRopeAttr(rope);

    pthread_mutex_unlock(&lock);
    //
    // Liberação do lock.
}

void EndNorth(rope_t * rope)
{
    // Início do lock.
    //
    pthread_mutex_lock(&lock);

    // Alteração do estado da corda.
    //
    rope->baboons_going_south--;
    rope->north_remaining_baboons--; 

    // A alteração acima representa, em termos do estado da corda, o fim da
    // travessia deste babuíno. Logo, como foi liberado um espaço na corda,
    // o próximo passo é sinalizar para que um babuíno, da mesma direção deste,
    // possa atravessar.
    //
    pthread_cond_signal(&waitNorth);

    // Se o número de babúinos atravessando em direção contrária a esta for igual
    // a zero (Corda livre, caso já não hajam babuínos atravessando nessa direção),
    // é dado um sinal para que os babuínos da outro lado possam atravessar.
    //
    if (rope->baboons_going_north == 0)
        pthread_cond_signal(&waitSouth);        

    pthread_mutex_unlock(&lock);
    //
    // Fim do lock.
}

//---------------------------------------------------------------------------
// Thread da travessia norte->sul. 
// Esta função representa a travessia de um único babuíno.
//
void *NorthSouthCrossing(void *arg)
{
    // Typecast de void* para rope_t*
    //
    rope_t *rope = (rope_t *) arg;
    
    // Identificador único e arbitrário para este babuíno, para facilitar
    // a visualização da concorrência na hora da execução do programa.
    //
    unsigned int baboonId = 0 + rand() % ( (100 + 1) - 0);
    printf("Baboon #N%u initialized.\n", baboonId);

    // Início da travessia norte->sul. 
    // Aqui é alterado o estado da corda e feita a sinalização entre as threads.
    //
    StartNorth(rope);

    // Execução da travessia:
    // Laço de repetição com duração de 5 segundos e sinalização na tela do 
    // momento da travessia. Note que neste ponto não há lock, de forma que o
    // procedimento pode ser realizado simultaneamente por várias threads, que é
    // o que buscamos em programas concorrentes.
    //
    for (int i = 1; i <= 5; i++)
    {
        printf("Babuino #N%u atravessou %u%% [", baboonId, 20*i);
        for (int j = 0; j < i; j++) printf("X");
        printf("]\n");
        delay_seconds(1);
    }
    
    // Fim da travessia norte->sul.
    // Novamente, alteração do estado da corda e sinalização entre as threads.
    //
    EndNorth(rope);

    return NULL;
}


// A partir deste ponto, tem-se a implentação das funções relativas à travessia
// sul-norte. É basicamente um espelho da implementação da travessia acima, por isso
// não é necessário comentá-la.

void StartSouth(rope_t * rope)
{

    static unsigned int consecutive_baboons = 0; 
    
    pthread_mutex_lock(&lock);

    while(rope->baboons_going_south > 0 ||
            rope->baboons_going_north >= *rope->capacity)
        pthread_cond_wait(&waitSouth, &lock);
 
    if (consecutive_baboons > 20)
    {
        pthread_cond_signal(&waitNorth);
        pthread_cond_wait(&waitSouth, &lock);
        consecutive_baboons = 0;
    }

    pthread_cond_signal(&waitSouth);

    rope->baboons_going_north++;
    consecutive_baboons++;

    printRopeAttr(rope);

    pthread_mutex_unlock(&lock);
}

void EndSouth(rope_t * rope)
{
    pthread_mutex_lock(&lock);

    rope->baboons_going_north--;
    rope->south_remaining_baboons--; 

    pthread_cond_signal(&waitSouth);

    if (rope->baboons_going_north == 0)
        pthread_cond_signal(&waitNorth);        

    pthread_mutex_unlock(&lock);
}

void *SouthNorthCrossing(void *arg)
{
    rope_t *rope = (rope_t *) arg;

    unsigned int baboonId = 0 + rand() % ( (100 + 1) - 0);
    printf("Baboon #S%u initialized.\n", baboonId);

    StartSouth(rope);

    for (int i = 1; i <= 5; i++)
    {
        printf("Babuino #S%u atravessou %u%% [", baboonId, 20*i);
        for (int j = 0; j < i; j++) printf("X");
        printf("]\n");
        delay_seconds(1);
    }
    
    EndSouth(rope);

    return NULL;
}


// O QUE FALTA
// FALAR NO README QUE O PRINT TÁ FEIO, MAS TA FUNCIONANDO
// FALAR COMO COMPILA
























//---------------------------------------------------------------------------
// Função principal.
//
int main()
{
    const unsigned int capacity = 5;
    
    srand((unsigned) time(NULL)); 

    // Estrutura do tipo corda. Esse é o recurso compartilhado entre as threads,
    // que devem acessá-lo com exclusão mútua, para o correto funcionamento do
    // programa.
    //
    rope_t rope = {
        .baboons_going_south = 0,
        .baboons_going_north = 0,
        .south_remaining_baboons = 20,
        .north_remaining_baboons = 20,
        .capacity = &capacity };

    // Vetor de pthread_t. Criado para que cada thread aponte para um elemento
    // único dessa lista.
    // ns -> norte-sul;
    // sn -> sul-norte
    //
    pthread_t ns[rope.north_remaining_baboons];
    pthread_t sn[rope.south_remaining_baboons];

    // Percorre o vetor de pthread_t e cria uma thread associada a cada um de
    // seus elementos. Cria tantas threads quanto a quantidade de macacos que 
    // precisam atravessar naquela direção.
    // Para todas, passa por referência a estrutura 'rope'.
    // 
    //
    for (int i = 0; i < rope.north_remaining_baboons; i++)
        pthread_create(&ns[i], NULL, NorthSouthCrossing, &rope);
    
    for (int i = 0; i < rope.south_remaining_baboons; i++)
        pthread_create(&sn[i], NULL, SouthNorthCrossing, &rope);
    
/*
    for (int i = 0; i < rope.north_remaining_baboons; i++)
        pthread_join(ns[i], NULL);
    
    for (int i = 0; i < rope.south_remaining_baboons; i++)
        pthread_join(sn[i], NULL);
*/

    // Lógica para inicializar aleatoriamente entre threads norte-sul e sul-norte. 
    //
    int a = 0;
    int b = 0;
    unsigned int rand_int;

    while( a < rope.north_remaining_baboons && 
            b < rope.south_remaining_baboons)
    {
        rand_int = rand();

        if (rand_int % 2 == 0)
        {
            if (a < rope.north_remaining_baboons)
            {
                pthread_join(ns[a], NULL);
                a++;
            }
        }
        else
        {
            if (b < rope.south_remaining_baboons)
            {
                pthread_join(sn[b], NULL);
                b++;
            }
        
        } 
    }


    // Este loop trava a main enquanto todas as demais threads não tiverem
    // finalizado, de forma a garantir que a thread main será a última a acabar.
    //
    while (rope.north_remaining_baboons > 0 || 
           rope.south_remaining_baboons > 0 ) 
        ;

    // Ao final da execução de todas as threads, o programa mostra o estado da
    // corda, que será sempre zero babuínos de cada lado, indicando o correto
    // funcionamento do programa.
    //
    printf("-------------------------\n");
    printf("= STATUS FINAL DA CORDA =\n");

    printRopeAttr(&rope);

    printf("--------------------\n");
    return 0;
}
