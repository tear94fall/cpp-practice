#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int make_socket_non_blocking(int sfd){
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);

    if(flags==-1){
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    
    if(s==-1){
        perror("fcntl");
        return -1;
    }

    return 0;
}

int create_and_bind(char* port){
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, retval;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    retval = getaddrinfo(NULL, port, &hints, &result);
    if(retval != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
        return -1;
    }

    for(rp = result; rp!=NULL; rp=rp->ai_next){
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sfd == -1){
            continue;
        }

        retval = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if(retval == 0){
            break;
        }

        close(sfd);
    }

    if(rp==NULL){
        fprintf(stderr, "Could not bind \n");
        return -1;
    }

    freeaddrinfo(result);

    return sfd;
}