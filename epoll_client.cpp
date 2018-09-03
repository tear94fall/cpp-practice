#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char ch='X';
    int portNumber;

    if(argc!=2){
        fprintf(stderr, "Usage: %s [portNumner]\n", argv[0]);
        return 1;
    }

    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(portNumber);

    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr*) &address, len);

    if(result == -1){
        perror("oops: cannot connect\n");
        return -1;
    }

    write(sockfd, &ch, 1);
    read(sockfd, &ch, 1);
    printf("response from server: %c\n", ch);
    close(sockfd);
    
    return 0;
}