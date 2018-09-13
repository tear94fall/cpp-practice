#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <errno.h>

#define NUM_OF_EVENTS   16
#define BUFSIZE     1024

enum mystate{
    THE_STATE_READ = 0,
    THE_STATE_WRITE
};

int main(int argc, char* argv[]){
    int mysocket;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t len;
    int sock;
    int n,i;
    struct epoll_event ev, ev_ret[NUM_OF_EVENTS];
    int epfd;
    int nfds;
    struct cinfo{
        int fd;
        char buf[1024];
        int n;
        int state;
    };

    if(argc!=2){
        fprintf(stderr, "Usage: %s [port number]\n", argv[0]);
        return 1;
    }

    int port_num = atoi(argv[1]);

    mysocket = socket(AF_INET, SOCK_STREAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_num);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    bind(mysocket, (struct sockaddr*) &addr, sizeof(addr));
    listen(mysocket, 5);

    epfd = epoll_create(NUM_OF_EVENTS);
    
    if(epfd<0){
        perror("epoll_create");
        return 1;
    }

    memset(&ev, 0, sizeof(ev));

    ev.events=EPOLLIN;
    ev.data.ptr=(cinfo *)malloc(sizeof(struct cinfo));

    if(ev.data.ptr==NULL){
        perror("malloc");
        return 1;
    }

    memset(ev.data.ptr, 0, sizeof(struct cinfo));

    ((struct cinfo*) ev.data.ptr)->fd = mysocket;

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, mysocket, &ev)!=0){
        perror("epoll_ctl");
        return 1;
    }

    for(;;){
        nfds = epoll_wait(epfd, ev_ret, NUM_OF_EVENTS, -1);
        if(nfds<0){
            perror("epoll_wait");
            return 1;
        }

        printf("after epoll wait: nfds = %d\n",nfds);

        for(i=0;i<nfds;i++){
            struct cinfo *ci = ((cinfo*)ev_ret[i].data.ptr);
            printf("fd=%d]n",ci->fd);

            if(ci->fd == mysocket){
                len=sizeof(client);
                sock=accept(mysocket, (struct sockaddr*)&client, &len);
                if(sock<0){
                    perror("accept");
                    return 1;
                }
                
                printf("accept sock=%d\n", sock);

                memset(&ev, 0, sizeof(ev));
                ev.events=EPOLLIN|EPOLLONESHOT;
                ev.data.ptr=malloc(sizeof(struct cinfo));
                if(ev.data.ptr == NULL){
                    perror("malloc");
                    return 1;
                }
                memset(ev.data.ptr, 0, sizeof(struct cinfo));
                ((struct cinfo*)ev.data.ptr)->fd = sock;

                if(epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev)!=0){
                    perror("epoll_ctl");
                    return 1;
                }
            }else{
                if(ev_ret[i].events &EPOLLIN){
                    ci->n = read(ci->fd, ci->buf, BUFSIZE);
                    if(ci->n<0){
                        perror("read");
                        return 1;
                    }
                    ci->state = THE_STATE_WRITE;
                    ev_ret[i].events=EPOLLOUT;

                    if(epoll_ctl(epfd, EPOLL_CTL_ADD, ci->fd, &ev_ret[i])!=0){
                        perror("epoll_ctl");
                        return 1;
                    }
                }else if(ev_ret[i].events & EPOLLOUT){
                    n=write(ci->fd, ci->buf , ci->n);
                    if(n<0){
                        perror("write");
                        return 1;
                    }

                    if(epoll_ctl(epfd, EPOLL_CTL_DEL, ci->fd, &ev_ret[i])!=0){
                        perror("epoll_ctl");
                        return 1;
                    }
                }else if(ev_ret[i].events &EPOLLOUT){
                    n=write(ci->fd, ci->buf, ci->n);
                    if(n<0){
                        perror("write");
                        return 1;
                    }

                    if(epoll_ctl(epfd, EPOLL_CTL_DEL, ci->fd, &ev_ret[i])!=0){
                        perror("epoll_ctl");
                        return 1;
                    }

                    close(ci->fd);
                    free(ev_ret[i].data.ptr);
                }
            }
        }
    }
    close(mysocket);
    return 0;
}