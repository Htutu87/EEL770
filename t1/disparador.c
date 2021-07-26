// EEL770 - Sistemas Operacionais
// UFRJ - 2021.1 - prof: Rodrigo Couto
// Autor: Artur Amaral | DRE: 119057968
// Descrição: Programa principal.

#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "tarefas.h"


bool esperaSinal = false;

void signalHandler(int numeroSinal)
{

    if (numeroSinal == SIGUSR1){
        // Executa tarefaUm();
        printf("Recebido SIGUSR1.\n");
        tarefaUm();
    }
    

    else if (numeroSinal == SIGUSR2){
        // Executa tarefaDois();
        printf("Recebido SIGUSR2.\n");
        tarefaDois();
    }
    

    else if (numeroSinal == SIGTERM){
        printf("Recebido SIGTERM.\n");
        esperaSinal = !esperaSinal;
    }
  
}

int main(){

    pid_t meuPid = getpid();
    //unsigned comandoParaExecutar = 0;
    
    printf("Disparador inicializado.\npid: %i\n", meuPid);

    signal(SIGTERM, signalHandler);
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);

    
    while (!esperaSinal){
        printf("Esperando sinal...\n");
        sleep(1);
    }

    printf("Finalizando o disparador.\n");

    return 0;
}
