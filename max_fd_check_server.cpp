#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LINE 1024
#define MAXCLIENT 1200
#define SERV_PORT 8787

int maxfdp1;
int chatuser;
unsigned int client_s[MAXCLIENT];
const char* escape = "/quit";

void disconCli(int i){
    close(client_s[i]);
    if(i==chatuser-1){
        client_s[i]=client_s[chatuser-1];
        chatuser--;
        printf("Now on the net %d user\n", chatuser);
    }
}

int maxuser(int user){
    unsigned int max = user;
    int i;

    for(i=0;i<chatuser;i++){
        if(max<client_s[i]){
            max=client_s[i];
        }
    }
    return max;
}

int main(void){
    char readline[LINE];
    const char* start = "Now you connected Server\n";
    int i,j,n;
    socklen_t addrlen;

    int serversocket, clientsocket, clilen;
    unsigned short port;
    fd_set read_fds;

    struct sockaddr_in client_addr, server_addr;
    int iError;

    if((serversocket = socket(AF_INET, SOCK_STREAM, 0))<0){
        printf("fail make socket\n");
        exit(0);
    }

    port = SERV_PORT;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_port=htons(port);

    if(bind(serversocket, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("fail bind\n");
        exit(0);
    }

    listen(serversocket, SOMAXCONN);
    maxfdp1=serversocket +1;

    printf("#######(%d) \n", FD_SETSIZE);

    while(1){
        FD_ZERO(&read_fds);
        FD_SET(serversocket, &read_fds);
        for(i=0;i<chatuser;i++){
            FD_SET (client_s[i],&read_fds);
        }

        maxfdp1=maxuser(serversocket)+1;

        if(select(maxfdp1, &read_fds, (fd_set*)0, (fd_set*)0, NULL)<0){
            printf("fail select function\n");
            exit(0);
        }

        if(FD_ISSET(serversocket,&read_fds)){
            addrlen =sizeof(client_addr);

            clilen=sizeof(client_addr);
            clientsocket = accept(serversocket, (struct sockaddr*)&client_addr, &addrlen);

            if(clientsocket == -1){
                printf("fail accept\n");
                exit(0);
            }

            client_s[chatuser] = clientsocket;
            chatuser++;

            send(clientsocket, start, strlen(start),0);

            printf("%d user connected\n", chatuser);
        }

        for(i=0;i<chatuser;i++){
            memset(readline, '\0', LINE);
            if(FD_ISSET(client_s[i], &read_fds)){
                if((n=recv(client_s[i], readline, LINE, 0))<=0){
                    disconCli(i);
                    continue;
                }

                if(strstr(readline, escape)){
                    disconCli(i);
                    continue;
                }

                readline[n]='\0';
                for(j=0;j<chatuser;j++){
                    send(client_s[i], readline, n, 0);
                }

                printf("%s\n", readline);
            }
        }
    }

    close(serversocket);
    
    return 0;
}