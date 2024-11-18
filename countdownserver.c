#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cnaiapi.h"
#include <unistd.h>  /* ADDED: for fork() and sleep() */
#include <signal.h>  /* ADDED: for signal handling */
#include <sys/wait.h>   // For waitpid
#include <sys/socket.h> // For send
#include "/home/home00/...Desktop/lab4/cnai-api64/cnaiapi/api/make_contact.c"
#include "/home/home00/.../Desktop/lab4/cnai-api64/cnaiapi/api/send_oef.c"


#define BUFFSIZE 256
#define INITIAL_COUNT 10


void handle_zombies(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    connection conn;
    char buff[BUFFSIZE];
    pid_t pid;  

 
    signal(SIGCHLD, handle_zombies);

    if (argc != 2) {
        (void) fprintf(stderr, "usage: %s <appnum>\n", argv[0]);
        exit(1);
    }

    
    while(1) {
        conn = await_contact((appnum) atoi(argv[1]));
        if (conn < 0)
            exit(1);

        pid = fork();
        
        if (pid < 0) {
            fprintf(stderr, "Fork failed\n");
            exit(1);
        }
        
        if (pid == 0) {  
            pid_t real_pid = getpid();
            printf("Countdown from server child\n");
            printf("Child PID: %d\n", real_pid);
            
            for (int count = INITIAL_COUNT; count >= 0; count--) {
                char msg[BUFFSIZE];
                snprintf(msg, sizeof(msg), "%d.%d\n", real_pid, count);
                (void) send(conn, msg, strlen(msg), 0);
                sleep(1);
            }
            

            char closing_msg[BUFFSIZE];
            snprintf(closing_msg, sizeof(closing_msg), 
                    "Countdown complete from PID %d\n", real_pid);
            (void) send(conn, closing_msg, strlen(closing_msg), 0);
            
    
            send_eof(conn);
            exit(0);
        }
        else {  
            /* Parent just continues to next iteration to accept new clients */
        }
    }

    return 0;
}
