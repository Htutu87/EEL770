/*
EEL770 - Sistemas Operacionais
UFRJ | 2021.1 | Prof.: Rodrigo Couto
Autor: Artur Amaral | DRE: 119057968
Descrição: Código fonte do disparador, o processo principal do trabalho.
*/

#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#define OK          0
#define MAX_BUFFER  sizeof(int)

bool esperaSinal = false;

/*-------------------------------------------------*/
/* Funções que implementam as tarefas requisitadas */
/*-------------------------------------------------*/

int tarefaUm()
{
    pid_t pidFilho;
    pidFilho = fork();

    if (pidFilho == 0){ 
        execl("./filhoUm", "filhoUm", NULL);
        return 0;
    }
    else {
        waitpid(pidFilho, NULL, 0); 
        printf("Tarefa 01 finalizada.\n");
        return 0;
    }

}

int tarefaDois()
{
    printf("Executando tarefa 02.\n");
    return 0;
}

/*---------------------------------------------*/
/* Função invocada no recebimento de um sinal  */
/*---------------------------------------------*/

void signalHandler(int numeroSinal)
{

    if (numeroSinal == SIGUSR1){
        printf("\n -> Recebido SIGUSR1.\n\n");
        tarefaUm();
    }
    
    else if (numeroSinal == SIGUSR2){
        printf("\n -> Recebido SIGUSR2.\n\n");
        tarefaDois();
    }
    
    else if (numeroSinal == SIGTERM){
        printf("\n -> Recebido SIGTERM.\n\n");
        esperaSinal = !esperaSinal;
    } 
}

/*---------------------------*/
/* Função principal          */
/*---------------------------*/

int main(){

    pid_t meuPid = getpid();
    //unsigned comandoParaExecutar = 0;
   
    printf("------------------------------------\n");
    printf("\n Disparador inicializado.\n\n\tpid: %i\n", meuPid);
    printf("\n------------------------------------\n");

    signal(SIGTERM, signalHandler);
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);

    do {
        printf("Esperando sinal...\n");
        sleep(1);
    } while (!esperaSinal);

    printf("Finalizando o disparador.\n");

    return 0;
}
