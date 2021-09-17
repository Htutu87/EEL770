
#include "baboons.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t waitNorth = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitSouth = PTHREAD_COND_INITIALIZER;

int t_count = 0;

void StartNorth(rope_t * rope)
{ 
    pthread_mutex_lock(&lock);
    while(rope->baboons_going_north > 0 ||
            rope->baboons_going_south >= *rope->capacity)
        pthread_cond_wait(&waitNorth, &lock);
    rope->baboons_going_south++;
    printRopeAttr(rope);
    pthread_mutex_unlock(&lock);

}

void EndNorth(rope_t * rope)
{
    pthread_mutex_lock(&lock);
    rope->baboons_going_south--;
    rope->north_remaining_baboons--; 
    pthread_cond_signal(&waitNorth);
    if (rope->baboons_going_north == 0)
        pthread_cond_signal(&waitSouth);        
    pthread_mutex_unlock(&lock);
}

void *NorthSouthCrossing(void *arg)
{
    rope_t *rope = (rope_t *) arg;
    unsigned int baboonId = 0 + rand() % ( (100 + 1) - 0);
    printf("Baboon #N%u initialized.\n", baboonId);

    t_count++;
    StartNorth(rope);

    for (int i = 1; i <= 5; i++)
    {
        printf("Babuino #N%u atravessou %u%% [", baboonId, 20*i);
        for (int j = 0; j < i; j++) printf("X");
        printf("]\n");
        delay_seconds(1);
    }
    
    EndNorth(rope);

    pthread_exit(NULL); 
    return NULL;
}


void StartSouth(rope_t * rope)
{ 
    pthread_mutex_lock(&lock);
    while(rope->baboons_going_south > 0 ||
            rope->baboons_going_north >= *rope->capacity)
        pthread_cond_wait(&waitSouth, &lock);
    pthread_cond_signal(&waitSouth);
    rope->baboons_going_north++;
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

    t_count++;
    StartSouth(rope);

    for (int i = 1; i <= 5; i++)
    {
        printf("Babuino #S%u atravessou %u%% [", baboonId, 20*i);
        for (int j = 0; j < i; j++) printf("X");
        printf("]\n");
        delay_seconds(1);
    }
    
    EndSouth(rope);

    pthread_exit(NULL); 
    return NULL;
}

int main()
{
    const unsigned int capacity = 5;

    rope_t rope = {
        .baboons_going_south = 0,
        .baboons_going_north = 0,
        .south_remaining_baboons = 20,
        .north_remaining_baboons = 20,
        .capacity = &capacity };

    pthread_t ns[rope.north_remaining_baboons];
    pthread_t sn[rope.south_remaining_baboons];

    for (int i = 0; i < rope.north_remaining_baboons; i++)
        pthread_create(&ns[i], NULL, NorthSouthCrossing, &rope);
    
    for (int i = 0; i < rope.south_remaining_baboons; i++)
        pthread_create(&sn[i], NULL, SouthNorthCrossing, &rope);
 


    for (int i = 0; i < rope.south_remaining_baboons; i++)
    {
        pthread_join(ns[i], NULL);
        pthread_join(sn[i], NULL);
    } 

    while (rope.north_remaining_baboons > 0 || 
           rope.south_remaining_baboons > 0 ) 
        ;

    printRopeAttr(&rope);

    return 0;
}
