#include <sys/poll.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    struct pollfd fds[3];
    int ifd1, ifd2, ofd, count;

    fds[0].fd=ifd1;
    fds[0].events=POLLIN;

    fds[1].fd=ifd2;
    fds[1].events=POLLIN;

    fds[2].fd=ofd;
    fds[2].events=POLLOUT;

    count = poll(fds, 3, 1000);
    if(count==-1){
        perror("poll failed");
        exit(1);
    }
    if(count == 0){
        printf("No data for reading or writing\n");
    }
    if(fds[0].revents & POLLIN){
        printf("there is data for reading fd %d\n", fds[0].fd);
    }
    if(fds[1].revents & POLLIN){
        printf("there is data for reading fd %d\n", fds[1].fd);
    }
    if(fds[2].revents & POLLOUT){
        printf("there is room to write on fd %d\n", fds[2].fd);
    }

    return 0;
}