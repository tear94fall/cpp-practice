#include <fcntl.h>  //read
#include <stdio.h>  //printf
#include <unistd.h> //open
#include <string.h> //memset
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <iostream>

#define EPOLL_MAX_CONN 2
#define EPOLL_RUN_TIMEOUT -1

int main(void){
    int epfd = epoll_create(EPOLL_MAX_CONN);
    int fd = open("test.txt", O_RDWR | O_NONBLOCK);

    if(fd>0){
        std::cout<<fd<<std::endl;
        
        const int BUFF_SIZE = 50;
        char buff[BUFF_SIZE];
        memset(buff, 0, BUFF_SIZE);

        struct epoll_event ev;
        struct epoll_event events;

        ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
        ev.data.fd = fd;

        int res = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

        if(res <0){
            printf("Error epoll_ctl: %i\n", errno);
        }

        int n = epoll_wait(epfd, &events , EPOLL_MAX_CONN, EPOLL_RUN_TIMEOUT);

        printf("Epoll unblocked\n");
        if(n<0){
            perror("Epoll failed\n");
        }else if(n==0){
            printf("TIMEOUT\n");
        }else{
            int size = read(fd, buff, BUFF_SIZE);

            if(size<0){
                printf("Error Reading the device: %s\n", strerror(errno));
            }else if (size>0){
                printf("Input found!\n");
            }else{
                printf("No input\n");
            }

            if( errno==EAGAIN){
                printf("ERRNO: EAGAIN\n");
            }
        }
    }

    return 0;
}