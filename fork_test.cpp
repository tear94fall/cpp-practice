#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

int global_var = 0;

int main(void){
    pid_t ret_pid, child;
    int local_var = 0;

    ret_pid = fork();
    if(ret_pid < 0){
        printf("fork() error\n");
        return -100;
    }else if(ret_pid == 0){
        global_var++;
        local_var++;
        printf("CHILD - my PID: %d parent PID: %d\n", getpid(), getppid());
    }else{
        sleep(10);
        global_var += 5;
        local_var += 5;
        printf("Parent - my pid:%d child's PID: %d\n", getpid(), ret_pid);
    }
    printf("\t global_var: %d\n", global_var);
    printf("\t local_var: %d\n", local_var);

    return 0;
}