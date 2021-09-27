/*

EEL770 - Sistemas Operacionais
UFRJ | 2021.1 | Prof.: Rodrigo Couto
Autor: Artur Amaral | DRE: 119057968
Descrição: Código fonte do disparador de processos.

*/

#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define OK              0
#define MAX_BUFFER      sizeof(int)
#define MIN_RAND        1
#define MAX_RAND        100
#define INDICE_LEITURA  0
#define INDICE_ESCRITA  1
#define SIGUSR1_FLAG    1
#define SIGUSR2_FLAG    2
#define SIGTERM_FLAG    3

/*-----------------------*/
/* Variáveis globais     */
/*-----------------------*/

bool esperarSinal = true;
int comandoParaExecutar = 0;
unsigned short sinalRecebido = 0;

/*-------------------------------------------------*/
/* Funções que implementam as tarefas requisitadas */
/*-------------------------------------------------*/

/*
 * A estrutura da TarefaUm(), no que se refere ao modo de uso das chamadas de
 * sistema e do estabeleciment do pipe, foram fortemente baseadas no que foi
 * ensinado em aula.  
 * */


pid_t TarefaUm()
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
       
        /* A seguinte lógica para obtenção de inteiros aleatórios dentro de um
         * intervalo (dado pelas macros), foi obtida no StackOverflow.*/

        srand((unsigned) time(&t));
        numeroSorteado = rand() % (MAX_RAND - MIN_RAND + 1) + MIN_RAND;

        printf("------------------------------------\n");
        printf("\n Executando tarefa 01 (pid: %u).\n", getpid());
        printf("\n\tNumero aleatorio sorteado: %i.\n", numeroSorteado);
        printf("\n------------------------------------\n\n");

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
        
        return pidFilho;
    }

}

pid_t TarefaDois()
{

    pid_t pidFilho = fork();

    /*printf("Comando para executar: %i\n", comandoParaExecutar);*/

    if (pidFilho == 0)
    {
        printf("------------------------------------\n");
        printf("\n Executando tarefa 02 (pid: %u).\n\n", getpid());

        if (comandoParaExecutar == 0)
            printf("\t--> Não há comando a executar\n");

        else if (comandoParaExecutar % 2 == 0)
            execlp("/bin/ping","ping","8.8.8.8","-c","5",NULL);

        else
            execlp("/bin/ping","ping","paris.testdebit.info","-c","5", "-i", "2", NULL); 
    }
    else {
        waitpid(pidFilho, NULL, 0);
        printf("\n------------------------------------\n\n");

    }

    return pidFilho;
}

/*---------------------------------------------*/
/* Função invocada no recebimento de um sinal  */
/*---------------------------------------------*/

/*
 *  Ao detectar um sinal, o código executa função signalHandler abaixo (Não
 *  encontrei uma bao tradução para português aqui). Uma condicional switch
 *  avalia qual sinal foi recebido, através do número que o identifica.
 *
 *  Ao entrar em uma das três possbilidades, uma variável global 'sinalRecebido'
 *  tem um valor arbitrário atribuido a si (representado pelas macros SIG_FLAG).
 *
 *  Esta atribuição tem a única função de controlar o fluxo do código uma vez que
 *  o programa volta a main(), determinando qual caminho será seguido nesta função
 *  após a detecção do sinal
 *  
 * */


void signalHandler(int numeroSinal)
{
    switch(numeroSinal){
        
        case SIGUSR1:                                   
            printf("\n --> SIGUSR1 detectado.\n\n");
            sinalRecebido = SIGUSR1_FLAG;
        break;
        
        case SIGUSR2:
            printf("\n --> SIGUSR2 detectado.\n\n");
            sinalRecebido = SIGUSR2_FLAG;
        break;
        
        case SIGTERM:
            printf("\n --> SIGTERM detectado.\n\n");
            sinalRecebido = SIGTERM_FLAG;
        break;
    }
}

/*---------------------------*/
/* Função principal          */
/*---------------------------*/

int main(){

    pid_t meuPid = getpid();

    printf("------------------------------------\n");
    printf("\n Disparador inicializado.\n\n\tpid: %i\n", meuPid);
    printf("\n------------------------------------\n\n");

    /* A seguir, tem-se as chamadas de sistema signal(), que permite meu código
     * identificar os sinais em questão. Este trecho tem forte inspiração da
     * referência sobre sinais passada no roteiro do trabalho, do site
     * www.thegeekstuff.com*/
 
    if (signal(SIGTERM, signalHandler) == SIG_ERR)
        printf("Erro registrando o sinal SIGTERM.\n");
    if (signal(SIGUSR1, signalHandler) == SIG_ERR)
        printf("Erro registrando o sinal SIGUSR1.\n");
    if (signal(SIGUSR2, signalHandler) == SIG_ERR)
        printf("Erro registrando o sinal SIGUSR2.\n");

    /* Abaixo tem-se o loop principal do disparador. A ideia é que ele ficará 
     * neste loop enquanto a variável global 'esperarSinal' for verdadeira.
     * As condições para que 'esperarSinal' torne-se falsa (e a subsequente
     * finalização da main) é a detectção do SIGTERM pelo processo principal ou
     * que o processo atual seja um filho do principal.
     *
     * Na execução das tarefas 01 e 02, o 'esperarSinal' é tornado falso pelos
     * processos filhos, de forma que, após realizarem suas respectivas tarefas,
     * eles saem do loop principal, voltando a main, em direção ao return final
     * do código. Após isso encerram-se suas próprias execuções e libera-se o
     * prosseguimento do processo pai (por conta da chamada waitpid(). )
     *
     * Este procedimento é implementado através do retorno das funções TarefaUm()
     * e TarefaDois(), que retornam o pid à main(). Caso seja retornado 0, o código
     * identifica que é um processo filho e falsea o 'esperarSinal', de modo a ser
     * finalizado.
     */


    do 
    {
        switch (sinalRecebido)
        {
            case (0):
                printf("Esperando sinal...\n");
                sleep(1);
            break;

            case (SIGUSR1_FLAG):
                if (TarefaUm() == 0)
                    esperarSinal = false;   
                sinalRecebido = 0;
            break; 
       
            case (SIGUSR2_FLAG):
                if (TarefaDois() == 0)
                    esperarSinal = false;
                sinalRecebido = 0;
            break;  
       
            case (SIGTERM_FLAG):
                printf("------------------------------------\n");
                printf("Finalizando o disparador.\n");
                printf("------------------------------------\n");
                esperarSinal = false;
            break; 
        }

    } while (esperarSinal);

    return OK;
}
