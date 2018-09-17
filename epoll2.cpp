#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>

int main(void){
    struct epoll_event event;
    int epoll_fd = epoll_create1(0);

    if(epoll_fd == -1){
        fprintf(stderr, "Failed to create epoll file desciptor\n");
        return 1;
    }

    event.events = EPOLLIN;
    event.data.fd = 0;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event)){
        fprintf(stderr, "Failed to add file descriptor to epoll\n");
        close(epoll_fd);
        return 1;
    }

    if(close(epoll_fd)){
        fprintf(stderr, "Failed to close epol file descriptor\n");
        return 1;
    }

    std::cout<<"epoll_create"<<std::endl;

    return 0;
}