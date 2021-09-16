#ifndef BABOONS_H
#define BABOONS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct
{
    unsigned int baboons_going_north,
                baboons_going_south,
                north_remaining_baboons,
                south_remaining_baboons;
    const unsigned int *capacity;
} rope_t;


void printRopeAttr(rope_t *rope)
{
    printf("Baboons going south: %u\n", rope->baboons_going_south);
    printf("Baboons going north: %u\n", rope->baboons_going_north);
}

void delay_seconds(double seconds)
{ 
    clock_t start_time;
    double elapsed_time = 0;

    start_time = clock();

    while (elapsed_time <= seconds)
        elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
}

#endif
