#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig);

int main(int argc, char* argv[]){
    if(signal(SIGINT, signal_handler)==SIG_ERR){
        printf("signal error\n");
        exit(1);
    }

    if(signal(SIGQUIT, signal_handler)==SIG_ERR){
        printf("signal error\n");
        exit(1);
    }

    for(;;){
        printf("main function is paused, PID = %d\n", getpid());
        pause();
    }

    return 0;
}

void signal_handler(int sig){
    if(sig==SIGINT){
        printf("SIGINT is transfered\n");
    }

    if(sig==SIGQUIT){
        printf("SIGQUIT is transfered\n");
        exit(1);
    }
}