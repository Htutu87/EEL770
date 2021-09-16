
#include "baboons.h"


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t waitNorth = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitSouth = PTHREAD_COND_INITIALIZER;


void GoNorth(rope_t * rope)
{ 
    pthread_mutex_lock(&lock);
 
    printf("rope->baboons_going_north: %u\n",rope->baboons_going_north );

    // A CONDIÇÃO NÃO ESTÁ SENDO SATISFEITA. grr
    while(rope->baboons_going_north > 0 ||
            rope->baboons_going_south >= *rope->capacity)
    {
        printf("To indo dormir.\n");
        pthread_cond_wait(&waitNorth, &lock);
        printf("Acordei.\n"); 
    }

    //printf("---\n[LOCKED]\n");
    //printf("Go north.\n");
   
    printf("--\n"); 
    rope->baboons_going_south++;
    printRopeAttr(rope);
    printf("--\n"); 
    
    //printf("[UNLOCKED]\n---\n");
    
    pthread_mutex_unlock(&lock);

}

void EndNorth(rope_t * rope)
{
    pthread_mutex_lock(&lock);

    //printf("---\n[LOCKED]\n");
    //printf("End north.\n");


    rope->baboons_going_south--;
    //printf("[UNLOCKED]\n---\n");
    rope->north_remaining_baboons--;
    
    //if (rope->baboons_going_south < *rope->capacity)
     pthread_cond_signal(&waitNorth);

    if (rope->baboons_going_north == 0)
        pthread_cond_signal(&waitSouth);
        
    pthread_mutex_unlock(&lock);
}


void *NorthSouthCrossing(void *arg)
{
    rope_t *rope = (rope_t *) arg;
    unsigned int baboonId = 0 + rand() % ( (100 + 1) - 0);

    GoNorth(rope);
    for (int i = 0; i < 3; i++)
    {
        printf("Babuino #%u atravessando.\n", baboonId);
        delay_seconds(1);
    }
    EndNorth(rope);
    pthread_exit(NULL); 
    return NULL;
}


// TO-DO
// MAIS MACACOS QUE A CAPACIDADE TÃO ATRAVESSANDOA  ACORDA.

int main()
{
    const unsigned int capacity = 5;

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
