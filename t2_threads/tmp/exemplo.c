#include <stdio.h>
#include <pthread.h>

typedef struct {
    int a;
    int b;
} myarg_t;

void *mythread(void *arg) 
{
    myarg_t *args = (myarg_t *) arg;
    printf("%d %d\n", args->a, args->b);
    return NULL;
}

int main(int argc, char *argv[]) 
{
    pthread_t p1;
    pthread_t p2;
   
    myarg_t args1 = { 10, 20 };
    myarg_t args2 = { 50, 25 };

    pthread_create(&p1, NULL, mythread, &args1);
    pthread_create(&p2, NULL, mythread, &args2);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    return 0;
}
