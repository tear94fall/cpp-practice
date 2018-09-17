#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define LINE 1024

int ProcessInputSend(int arg);

char userID[10];

char *usrNum;

char line[LINE], chatdata[LINE+1];
struct sockaddr_in server_addr;

int serversocket;

const char* escape ="/quit";

int bIsQuit;

int main(int argc, char *argv[]){
    int size;
    usrNum = argv[1];
    
    bIsQuit=FALSE;

    if((serversocket = socket(AF_INET, SOCK_STREAM,0))<0){
        printf("fail make socket\n");
        exit(0);
    }

    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr= inet_addr("127.0.01");
    server_addr.sin_port = htons(8787);

    if(connect(serversocket, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("fail connect to server\n");
        exit(0);
    }else{
        printf("connected server\n");
    }

    ProcessInputSend(0);

    while(!bIsQuit){
        bzero(chatdata, sizeof(chatdata));

        if((size=recv(serversocket, chatdata, LINE, 0))==0){
            printf("recv ERROR\n");
            exit(0);
        }else{
            chatdata[size]='\0';
            printf("%s\n", chatdata);
        }
    }
    
    close(serversocket);
}

int ProcessInputSend(int arg){
    while(1){
        if(fgets(chatdata, LINE, stdin)){
            chatdata[strlen(chatdata)-1]='\0';
            sprintf(line, "[user number is: %s]: %s", usrNum, chatdata);
            if(send(serversocket, line, strlen(line),0)<0){
                printf("Write fail\n");
            }

            if(strstr(line, escape)!=0){
                printf("Bye\n");
                close(serversocket);
                bIsQuit = TRUE;
                exit(0);
            }
        }
    }
}