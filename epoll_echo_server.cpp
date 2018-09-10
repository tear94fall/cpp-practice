#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

//not work, needs modify....

#define EPOLL_SIZE 60
#define EPOLL_EVENT_SIZE 100

int setnonblock(int sock);
int settcpnodelay(int sock);
int setreuseaddr(int sock);
int get_portnumber_from_arg(int argc, char* argv[]);
int init_epoll(int eventsize);
int init_acceptsock(unsigned short port);
int do_use_fd(int efd, struct epoll_event ev);
int do_accept(int efd, int sfd);
int do_add_fd(int efd, int cfd);
int do_modify_fd(int efd, int cfd);
int do_del_fd(int efd, int cfd);

//main function
int main(int argc, char** argv){
    const int poolsize = EPOLL_SIZE;
    const int epollsize = EPOLL_EVENT_SIZE;
    struct epoll_event *events;
    struct epoll_event ev;
    int sfd, efd;
    int i;
    int max_got_events;
    int result;
    int port;

    port = get_portnumber_from_arg(argc, argv);

    if(port<0){
        puts("I need 1 argument (as a listen port)");
        return 1;
    }

    efd = init_epoll(epollsize);
    
    if(efd<0){
        perror("init_epoll error");
        return 1;
    }

    //init pool
    events = (struct epoll_event*)malloc(sizeof(*events)*poolsize);
    if(NULL==events){
        perror("epoll_create(0) error");
        return -1;
    }

    sfd = init_acceptsock(port);
    if(sfd <0){
        perror("init_acceptsock error");
        return 1;
    }

    printf("Running Server port %d\n", port);
    result = do_add_fd(efd, sfd);
    if(result <0){
        perror("epoll_ctl error");
        return 1;
    }

    while(1){
        max_got_events = epoll_wait(efd, events, poolsize, -1);

        for(i=0;i<max_got_events;i++){
            if(events[i].data.fd==sfd){
                do_accept(efd, sfd);
            }else{
                do_use_fd(efd, events[i]);
            }
        }
    }
    return 1;
}


int setnonblock(int sock){
    int flags = fcntl(sock, F_GETFL);
    flags |= O_NONBLOCK;
    if(fcntl (sock, F_SETFL, flags)<0){
        perror("fcntl, executing nonblock error");
        return -1;
    }
    return 0;
}

int settcpnodelay(int sock){
    int flag=1;
    int result = setsockopt(sock,
    IPPROTO_TCP,
    TCP_NODELAY,
    (char*) &flag,
    sizeof(int));

    return result;
}

int setreuseaddr(int sock){
    int flag = 1;
    int result = setsockopt(sock,
    SOL_SOCKET,
    SO_REUSEADDR,
    (char*)&flag,
    sizeof(int));

    return result;
}

int get_portnumber_from_arg(int argc,char *argv[]){
    if(argc==2){
        return atoi(argv[1]);
    }else{
        return -1;
    }
}

int init_epoll(int eventsize){
    int efd;

    if((efd==epoll_create(eventsize))<0){
        perror("epoll_create(1) error");
        return -1;
    }

    return efd;
}

int init_acceptsock(unsigned short port){
    struct sockaddr_in addr, clientaddr;

    int client = sizeof(clientaddr);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sfd==-1){
        perror("socket error");
        close(sfd);
        return -1;
    }

    setreuseaddr(sfd);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sfd, (struct sockaddr*)&addr, sizeof(addr))==-1){
        close(sfd);
        return -2;
    }
    
    listen(sfd, 5);
    return sfd;
}


int do_use_fd(int efd, struct epoll_event ev){
    int readn;
    int sendflags=0;
    char buf_in[1024]={'\0'};
    int cfd = ev.data.fd;

#ifdef USE_ET
    int sizen =0;
    char readbuf[256]={'\0'};

    while(1){
        readn=read(cfd, readbuf, 255);
        if(readn<0){
            if(EAGAIN == errno){
                break;
            }

            do_del_fd(efd, cfd);
            close(cfd);
            printf("Close fd %d by %d\n", cfd, readn);
            perror("Closed");
            return -1;
        }

        sizen+=readn;
        if(sizen >=1024){
            do_del_fd(efd, cfd);
            close(cfd);
            printf("Close buffer full fd %d by %d\n", cfd, readn);
            perror("Closed");
            return -1;
        }
        memset(buf_in, readbuf,readn);
    }
    printf("read data %d, %s",sizen,buf_in);
    send(cfd, buf_in, strlen(buf_in_, sendflasgs));

    #ifdef USE_ETONESHOT

    #endif
    #else

    readn = read(cfd, buf_in, 255);
    if(readn<=0){
        do_del_fd(efd, cfd);
        close(cfd);
        printf("Close %d", cfd);
    }else{
        printf("read data %s", buf_in);
        send(cfd, buf_in, strlen(buf_in),sendflags);
    }
    #endif
    return 1;
}

int do_accept(int efd, int sfd){
    int cfd;
    socklen_t client;
    struct sockaddr_in addr, clientaddr;

    printf("Accept\n");
    cfd = accept(sfd, (struct sockaddr*)&clientaddr, &client);
    if(cfd<0){
        perror("Accept error");
        return -1;
    }

    #ifdef USE_ET
    setnonblock(cfd);
    #endif
    settcpnodelay(cfd);

    do_add_fd(efd, cfd);

    return cfd;
}

int do_add_fd(int efd, int cfd){
    struct epoll_event ev;

    ev.events = EPOLLIN;
    #ifdef USE_ET
    ev.events |= EPOLLET;
    #ifdef USE_ETONESHOT
    ev.events|=EPOLLONESHOE;
    #endif
    #endif
    ev.data.fd=cfd;
    return epoll_ctl(efd, EPOLL_CTL_ADD, cfd , &ev);
}


int do_modify_fd(int efd, int cfd){
    struct epoll_event ev;

    ev.events=EPOLLIN;
    #ifdef USE_ET
    ev.events |= EPOLLET;
    #ifdef USE_ETONESHOT
    ev.events|=EPOLLONESHOT;
    #endif
    #endif

    ev.data.fd=cfd;
    return epoll_ctl(efd, EPOLL_CTL_MOD, cfd, &ev);
}


int do_del_fd(int efd, int cfd){
    struct epoll_event ev;
    return epoll_ctl(efd,EPOLL_CTL_DEL, cfd, &ev);
}
