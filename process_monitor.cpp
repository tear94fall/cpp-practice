#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

char g_sExeFile[200];
char g_sExePath[200];
int g_nChildPid;

void init(){
    memset(g_sExeFile,0,200);
    memset(g_sExePath,0,200);

    strcpy(g_sExePath,"C:\\");
    strcpy(g_sExeFile,"test.txt");
}

void starter(){
    g_nChildPid = fork();
    if(g_nChildPid<0){
        printf("[START][FORK] process fork fails\n");
        printf("[SHUTDOWN] Monitor closed\n");
        exit(0);
    }

    if(g_nChildPid==0){
        if(execl(g_sExePath, g_sExeFile,(char*)0)<0){
            printf("[EXEC] process execution fails\n");
            exit(0);
        }
    }
    printf("[START] %s process started. pid=%d\n", g_sExeFile, g_nChildPid);
}

int main(int argc, char *argv[]){
    int nChildPid;
    long ctime, otime;

    init();
    signal(SIGCLD, SIG_IGN);

    starter();

    ctime=otime=time(NULL);

    while(1){
        ctime = time(NULL);

        if((ctime-otime)>30){
            if(kill(g_nChildPid,0)<0){
                printf("[MOM] process stopped %d\n", g_nChildPid);
                starter();
            }else{
                printf("[MOM] process is healthy %d\n", g_nChildPid);
            }
            otime = ctime;
        }
        sleep(1);
    }
}