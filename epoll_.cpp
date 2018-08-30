#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/epoll.h>

#define MAX_BUFFER_LEN 256
#define MAX_EVENT_NUM 1

int main(int argc, char* argv[]){
    int retval, m_read, m_write;
    int i;
    int fd;
    int epfd;
    struct epoll_event ev;
    struct epoll_event evlist[MAX_EVENT_NUM];
    char buffer[MAX_BUFFER_LEN];

    if((fd=open("./temp.txt", O_RDWR))==-1){
        perror("open: ");
        exit(EXIT_FAILURE);
    }

    /* epoll create */
    epfd = epoll_create(1);
    if(epfd == -1){
        perror("epoll_create: ");
        exit(EXIT_FAILURE);
    }

    /* epoll control */
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1){
        perror("epoll_ctl: ");
        exit(EXIT_FAILURE);
    }

    while(1){
        memset(buffer, 0x00, MAX_BUFFER_LEN);
        retval = epoll_wait(epfd, evlist, MAX_EVENT_NUM, -1);
        if(retval == -1){
            /* signal inerrupt*/
            if(errno == EINTR){
                continue;
            }else{
                perror("epoll_wait: ");
                exit(EXIT_FAILURE);
            }
        }
        printf("retval = %d \n", retval);

        for(i = 0; i< MAX_EVENT_NUM; i++){
            if(evlist[i].events & EPOLLIN){
                m_read = read(STDIN_FILENO, buffer, MAX_BUFFER_LEN);
                printf("[read] m_read = %d \n", m_read);

                m_write = write(fd, buffer, m_read);
                printf("[write] m_write = %d \n", m_write);
            }
        }
        usleep(1000);
    }
    close(fd);
    exit(EXIT_FAILURE);
}