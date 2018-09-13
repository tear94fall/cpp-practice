#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

void error_handling(const char* message);

int main(void){
    int sock;
    int snd_buf, rcv_buf;
    int state;
    socklen_t len;

    sock=socket(PF_INET, SOCK_STREAM, 0);

    len=sizeof(snd_buf);

    state=getsockopt(sock,SOL_SOCKET, SO_SNDBUF, &snd_buf, &len);

    if(state){
        error_handling("getsockopt() error");
    }


    len=sizeof(rcv_buf);
    state=getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcv_buf, &len);
    if(state){
        error_handling("getsockopt() error");
    }

    printf("데이터 입력 받기 위한 소켓의 버퍼 크기: %d(수신버퍼)\n", rcv_buf);
    printf("데이터 출력 받기 위한 소켓의 버퍼 크기: %d(송신버퍼)\n",snd_buf);

    return 0;
}

void error_handling(const char* message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}