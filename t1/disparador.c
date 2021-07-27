/*
EEL770 - Sistemas Operacionais
UFRJ | 2021.1 | Prof.: Rodrigo Couto
Autor: Artur Amaral | DRE: 119057968
Descrição: Código fonte do disparador, o processo principal do trabalho.
*/

#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define OK              0
#define MAX_BUFFER      80//sizeof(int)
#define MIN_RAND        1
#define MAX_RAND        100
#define INDICE_LEITURA  0
#define INDICE_ESCRITA  1
#define SIGUSR1_FLAG    1
#define SIGUSR2_FLAG    2
#define SIGTERM_FLAG    3

bool esperaSinal = true;
unsigned short sinalRecebido = 0;
int comandoParaExecutar = 0;

/*-------------------------------------------------*/
/* Funções que implementam as tarefas requisitadas */
/*-------------------------------------------------*/

pid_t tarefaUm()
{
    pid_t pidFilho;
    int descritoresPipe[2];
    int bufferLeitura[MAX_BUFFER];

    time_t t;
    int numeroSorteado;
     
    pipe(descritoresPipe); 
    pidFilho = fork();

    if (pidFilho == 0)
    {
        close(descritoresPipe[INDICE_LEITURA]);
        
        srand((unsigned) time(&t));
        numeroSorteado = rand() % (MAX_RAND - MIN_RAND + 1) + MIN_RAND;

        printf("------------------------------------\n");
        printf("\n Executando tarefa 01 (pid: %u).\n", getpid());
        printf("\n\tNumero aleatorio sorteado: %i.\n", numeroSorteado);
        printf("\n------------------------------------\n");

        write(descritoresPipe[INDICE_ESCRITA], &numeroSorteado, sizeof(numeroSorteado));
        close(descritoresPipe[INDICE_ESCRITA]);

        return pidFilho;
    }

    else 
    {
        close(descritoresPipe[INDICE_ESCRITA]);
        waitpid(pidFilho, NULL, 0);

        read(descritoresPipe[INDICE_LEITURA], bufferLeitura, MAX_BUFFER);
        comandoParaExecutar = *bufferLeitura;
        close(descritoresPipe[INDICE_LEITURA]); 
        printf("Valor recebido do filho: %i\n", comandoParaExecutar);
        comandoParaExecutar = *bufferLeitura;
        return pidFilho;
    }

}

pid_t tarefaDois()
{

    pid_t pidFilho = fork();

    printf("Comando para executar: %i\n", comandoParaExecutar);

    if (pidFilho == 0)
    {
        if (comandoParaExecutar == 0){
            printf("Não há comando a executar\n");
        }
        else if (comandoParaExecutar % 2 == 0){
            execlp("/bin/ping","ping","8.8.8.8","-c","5",NULL);
        }
        else {
            execlp("/bin/ping","ping","paris.testdebit.info","-c","50", "-i", "2", NULL);
        }
    }
    else {
        waitpid(pidFilho, NULL, 0);
    }

    return pidFilho;
}

/*---------------------------------------------*/
/* Função invocada no recebimento de um sinal  */
/*---------------------------------------------*/

void signalHandler(int numeroSinal)
{

    switch(numeroSinal){
        
        case SIGUSR1:                                   
            printf("\n -> Recebido SIGUSR1.\n\n");
            sinalRecebido = SIGUSR1_FLAG;
        break;
        
        case SIGUSR2:
            printf("\n -> Recebido SIGUSR2.\n\n");
            sinalRecebido = SIGUSR2_FLAG;
        break;
        
        case SIGTERM:
            printf("\n -> Recebido SIGTERM.\n\n");
            sinalRecebido = SIGTERM_FLAG;
        break;
    }

    /*
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
    } */
}

/*---------------------------*/
/* Função principal          */
/*---------------------------*/

int main(){

    pid_t meuPid = getpid();

    printf("------------------------------------\n");
    printf("\n Disparador inicializado.\n\n\tpid: %i\n", meuPid);
    printf("\n------------------------------------\n");

    signal(SIGTERM, signalHandler);
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);

    do 
    {
        switch (sinalRecebido)
        {
            case (0):
                printf("Esperando sinal...\n");
                sleep(1);
            break;

            case (SIGUSR1_FLAG):
                if (tarefaUm() == 0)
                 esperaSinal = false;   
                sinalRecebido = 0;
            break; 
       
            case (SIGUSR2_FLAG):
                if (tarefaDois() == 0)
                    esperaSinal = false;   
                sinalRecebido = 0;
            break;  
       
            case (SIGTERM_FLAG):
                printf("Finalizando o disparador.\n");
                esperaSinal = false;
            break; 
        }

    } while (esperaSinal);

    return OK;
}
