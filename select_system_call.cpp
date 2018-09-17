#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#define MAX_BUF 256

int port_number = 2000;

void child_process(void){
    sleep(2);
    char msg[MAX_BUF];
    struct sockaddr_in addr={0};
    srandom(getpid());

    int n, sockfd, num = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_number);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    printf("Child {%d} connected\n", getpid());

    while(1){
        int sl = (random() %10)+1;
        num++;
        sleep(sl);
        sprintf(msg, "Test message %d from client %d", num, getpid());
        n=write(sockfd, msg, strlen(msg));
    }
}

int main(int argc, char* argv[]){
    char buffer[MAX_BUF];
    int fds[5];
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t addr_len;
    int n, i, max = 0;
    int sockfd, commfd;

    fd_set rset;
    for(i=0;i<5;i++){
        if(fork()==0){
            child_process();
            exit(0);
        }
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_number);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 5);

    for(i=0;i<5;i++){
        memset(&client, 0, sizeof(client));
        addr_len = sizeof(client);
        fds[i] = accept(sockfd, (struct sockaddr*)&client, &addr_len);
        if(fds[i]>max){
            max = fds[i];
        }
    }

    while(1){
        FD_ZERO(&rset);
        for(i=0;i<5;i++){
            FD_SET(fds[i], &rset);
        }

        puts("round again");
        select(max+1, &rset, NULL, NULL, NULL);

        for(i=0;i<5;i++){
            if(FD_ISSET(fds[i], &rset)){
                memset(buffer, 0, MAX_BUF);
                read(fds[i], buffer, MAX_BUF);
                puts(buffer);
            }
        }
    }

    return 0;
}