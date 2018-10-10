/**
 * A Program that spawns k number of processes
 * and connects them all via pipes to send
 * a message from process 1 to a given dest.
 *
 * @author Dylan Kernohan
 *
 * This program takes 2 arguments.
 * argv[0] is the virtual PID of the first process
 * argv[1] is the number of processes to be spawned.
 *
 * EX call: ./a.out 1 5
 * This will spawn processes PID # 1, 2, 3, 4, 5
 * */

#include <stdio.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>


#define READ 0
#define WRITE 1
#define MAX 1024

void sigHandler(int sigNum);

/* create a structure that will be passed from process to process via pipes */
struct dataStruct {
    int dest;               // Teh destination for the message
    char message[50];       // The message wanting to be sent. Max length 50
    FILE* file;
};

struct dataStruct* data;

/* Where all the magic happens */
int main(int argc, char* argv[]){


    /*TODO A level solution: Have initial process create an extra pipe, 
     * have every child process close the fd's to that pipe,
     * have the last process leave them open connecting first process to last.
     */


    ssize_t bytes_read;     // Used to store the number of bytes read from read()
    char str[MAX];          // A String to hold user input
    int fd[2];              // Array to hold the initial fd's of a given process.
    pid_t pid;              // The PID of the child created by fork()
    int processNum = atoi(argv[1]); // to keep track of what number processs this is.

    /* Override ^C signal */
    signal(SIGINT, sigHandler);

    /* allocate memory for the data scruct */
    data = malloc(sizeof(struct dataStruct));

    /* create a pipe */
    if(pipe(fd) < 0){
        perror ("Error creating pipe\n");
        exit(1);
    }

    /* Create pipeline structure by forking. argv[2] is the number of process still needed */
    int numProcessesNeeded = atoi(argv[2]);
    if(numProcessesNeeded > 1){

        pid = fork();

        if(pid < 0){
            perror("there was an exception\n");
        }else if (pid){

            /****************************
             * This is the parent code  *
             ****************************/

            /* dup the fd we want to keep storing it in fd 6*/
            dup2(fd[WRITE], 6);

            /* close the two fd's the pipe made. freeing up fd 3 and 4 for next time */
            close(3);
            close(4);

        }else{

            /****************************
             * This is the child code   *
             ****************************/

            /* close the 5th fd that was copied from parent */
            close(5);

            /* dup the fd we want to keep storing it in fd 5
             * Could have explicitly called dup2(5, fd[READ])*/
            dup(fd[READ]);

            /* close Write fd */
            close(3);
            close(4);

            /* recursive exec logic, making the child the parent */
            char *args[256];            // Array of pointers that point to each argument
            char *file;                 // Pointer to the file name
            char nextProcessString[20];
            char newNumProcessesNeededString[20];
            int nextProcessNum = processNum + 1;

            sprintf(nextProcessString, "%d", nextProcessNum);
            sprintf(newNumProcessesNeededString, "%d", (numProcessesNeeded - 1));

            file = "./a.out";
            args[0] = file;
            args[1] = nextProcessString;
            args[2] = newNumProcessesNeededString;
            args[3] = NULL;

            int execresult;
            if((execresult = execvp(file, args)) < 0){
                perror("exec failed");
                exit(1);
            }
        }
}

    /* Code that every process runs after setup. This is where read/write occur */
    while(1){

        /* Check if it is the first process to start the chain since this solution does not loop */
        if(processNum == 1){
            sleep(1);
            printf("Enter Message: ");
            fflush(stdout);
            fgets(str, MAX, stdin);
            strcpy(data->message, str);


            printf("Enter Destination: ");
            fflush(stdout);
            fgets(str, MAX, stdin);
            data -> dest = atoi(str);

            printf("(PID: %d) - I am living process #%d - (PPID: %d)\n", getpid(), processNum, getppid());
            printf("\tI Wrote: %s", data -> message);
            printf("----------------------------------------------\n");
            write(6, (const void *) data, (size_t) sizeof(struct dataStruct));
        }else{

            /* Every other process runs this path */

            /* read from the previous pipe) */
            bytes_read = read(5, data, (size_t) sizeof(struct dataStruct));
            if(bytes_read > MAX){
                perror ("pipe read error\n");
                exit(1);
            }

            /* Display process info to STDOUT */
            printf("(PID: %d) - I am living process #%d - (PPID: %d)\n", getpid(), processNum, getppid());
            printf("\tI Read: %s", data -> message);

            /* Check if the process that just read is the dest */
            /* if so, do not write message any further so we can start over */
            if(processNum == data -> dest){
                printf("\tEnd of the line! Start over.\n");
                printf("----------------------------------------------\n");
                //data -> dest = 1;
            }else{

                /* if not dest, keep writing the message on to the next process */

                /* write the next pipe*/
                write(6, (const void *) data, (size_t) sizeof(struct dataStruct));
                printf("\tI Wrote: %s", data -> message);
                printf("----------------------------------------------\n");
            }
        }
    }

    return 0;
}

/* This function overrides each processe SIGINT signal for shut down. */
void sigHandler (int sigNum){

    printf("\r ^C recieved. Process %d shutting down.\n", getpid());
    /* free memory that was allocated for data struct */
    free(data);
    exit(0);
}
