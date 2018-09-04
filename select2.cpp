#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>

int main(void){
    int fd[2];
    int i;
    int n;
    int state;

    char buf[255];

    struct timeval tv;

    fd_set readfds, writefds;

    if((fd[0]=open("/tmp/testfile", O_RDONLY))==-1){
        perror("file open error");
        exit(0);
    }

    if((fd[1]=open("tmp/testfile2", O_RDONLY))==-1){
        perror("file open error");
        exit(0);
    }

    for(;;){
        FD_ZERO(&readfds);
        FD_SET(fd[0], &readfds);
        FD_SET(fd[1], &readfds);

        state = select(fd[1]+1, &readfds, NULL,NULL,NULL);
        switch(state){
            case -1:
                perror("select error: ");
                exit(0);
                break;
            default:
                for(i=0;i<2;i++){
                    while((n=read(fd[i], buf, 255))>0){
                        printf("(%d) [%d] %s",state, i, buf);
                    }
                }
            memset(buf, 0x00, 255);
            break;
        }
        usleep(1000);
    }
}