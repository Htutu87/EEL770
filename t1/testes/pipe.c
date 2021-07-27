// Teste sobre pipes.
// Artur Amaral.


#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


int main(){
    
    int childPid;
    int fd[2]; // File descriptor for the pipe. Two elements int array.
    char string[] = "Hello, World!\n";
    char readBuffer[80];


    pipe(fd);
    childPid = fork();

    if (childPid == 0){
        // Son.
        close(fd[0]); 
        write(fd[1], string, strlen(string)+1);
        close(fd[1]);
        for (int i = 0; i < 4; i++)sleep(1);
        
    }

    else {
        // Father.
        close(fd[1]);
        waitpid(childPid, NULL, 0);
        read(fd[0], readBuffer, sizeof(readBuffer));
        printf("%s", readBuffer);
        close(fd[0]);
        while(1)sleep(1);
    }
 
    return 0;
}
