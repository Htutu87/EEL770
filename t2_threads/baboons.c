
#include "baboons.h"


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t waitNorth = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitSouth = PTHREAD_COND_INITIALIZER;


void GoNorth(rope_t * rope)
{ 
    pthread_mutex_lock(&lock);
    printf("---\n[LOCKED]\n");
    rope->baboons_going_north++;
    printf("Go north.\n");
    printRopeAttr(rope);
    printf("[UNLOCKED]\n---\n");
    pthread_mutex_unlock(&lock);

}

void EndNorth(rope_t * rope)
{
    pthread_mutex_lock(&lock);
    printf("---\n[LOCKED]\n");
    rope->baboons_going_north--;
    printf("End north.\n");
    printRopeAttr(rope);
    printf("[UNLOCKED]\n---\n");
    rope->north_remaining_baboons--;
    pthread_mutex_unlock(&lock);
}


void *NorthSouthCrossing(void *arg)
{
    rope_t *rope = (rope_t *) arg;

    GoNorth(rope);
    EndNorth(rope);
    pthread_exit(NULL); 
}


int main()
{
    const unsigned int capacity = 5;
    // FALTA UMA VARIÁVEL QUE INDICA A QUANTIDADE DE BABUINOS QUE SOBRARAM EM
    // CADA LADO.

    rope_t rope = {
        .baboons_going_south = 0,
        .baboons_going_north = 0,
        .south_remaining_baboons = 100,
        .north_remaining_baboons = 100,
        .capacity = &capacity };

    pthread_t ns;

    for (int i = 0; i < rope.north_remaining_baboons; i++)
    {
        pthread_create(&ns, NULL, NorthSouthCrossing, &rope);
    }

    pthread_join(ns, NULL);
        
    return 0;
}
