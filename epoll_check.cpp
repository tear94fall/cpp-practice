#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    int epoll_fd;

    if((epoll_fd = epoll_create(500))==-1){
        printf("epoll 파일 스크립터 생성오류\n");
    }else{
        printf("epoll 파일 스크립터 생성성공\n");
        close(epoll_fd);
    }

    return 0;
}