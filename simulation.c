#include <stdio.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(){

    
    /* currently thinking instead of waint, creating a pipe from parent to child. 
     * Only is a pipe is blocking though. Then in the child code, it will keep
     * executing another simulation.c until k children exist
     * */

    pid_t PID = fork();
    int status;
    if(PID < 0){
        perror("there was an exception");
    }else if (PID){
      
        wait(&status);

    }else{

        char *argv[256];            // Array of pointers that point to each argument
        char *file;                 // Pointer to the file name

        // Use these values to test exec call
        file = "ls";
        argv[0] = file;
        argv[1] = "-l";

        int execresult;
        if((execresult = execvp(file, argv)) < 0){
            perror("exec failed");
            exit(1);
        }
        
    }
    return 0;
}



