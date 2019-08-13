
/*
 client
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

int main(int argc, char *argv[]){
    int sock;
    ssize_t bytes_received;
    char send_data[1024], recv_data[1024];
    struct sockaddr_in server_addr;
    
    struct hostent *host = gethostbyname("127.0.0.1");
    
    if((sock=socket(AF_INET, SOCK_STREAM, 0))==-1){
        perror("Socket");
        exit(1);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(9999);
    memset(&(server_addr.sin_zero), 0x00, 8);
    
    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr))==-1){
        perror("Connect");
        return 0;
    }
    
    while(1){
        printf("SEND: ");
        fgets(send_data, sizeof(send_data), stdin);
        
        if((strcmp(send_data, "quit\n")) != 0){
            send(sock, send_data, strlen(send_data), 0);
        }else{
            send(sock, send_data, strlen(send_data), 0);
            printf("close connection\n");
            close(sock);
            break;
        }
        
        bytes_received = recv(sock, recv_data, 1024, 0);
        recv_data[bytes_received] = '\0';
        
        if((strcmp(recv_data, "quit\n")) == 0){
            printf("close connection");
            close(sock);
            break;
        }else{
            printf("RECV: %s", recv_data);
        }
    }
    
    return 0;
}
