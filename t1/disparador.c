// EEL770 - Sistemas Operacionais
// UFRJ - 2021.1 - prof: Rodrigo Couto
// Autor: Artur Amaral | DRE: 119057968
// Descrição: Programa principal.


#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

bool esperaSinal = false;

int tarefaUm(){
    printf("Inside tarefaUm() function.\n");
    return 0;
}

int tarefaDois(){
    printf("Inside tarefaDois() function.\n");
    return 0;
}

void signalHandler(int numeroSinal)
{

    if (numeroSinal == SIGUSR1){
        // Executa tarefaUm();
        printf("received SIGUSR1\n");
        tarefaUm();
    }
    

    else if (numeroSinal == SIGUSR2){
        // Executa tarefaDois();
        printf("received SIGUSR2\n");
        tarefaDois();
    }
    

    else if (numeroSinal == SIGTERM)
        esperaSinal = !esperaSinal;
  
}

int main(){

    pid_t meuPid = getpid();
    //unsigned comandoParaExecutar = 0;
    
    printf("Disparador inicializado.\npid: %i\n", meuPid);

    signal(SIGTERM, signalHandler);
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);

    
    while (!esperaSinal){
        printf("Inside main function\n");
        sleep(1);
    }

    printf("Finalizando o disparador.\n");

    return 0;
}
