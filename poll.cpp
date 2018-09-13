#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <poll.h>

#define MAX_BUFFER_LEN 256

int main(int argc, char* argv[]){
    int retval, m_read, m_write;
    int fd;
    struct pollfd pollfds[1];
    char buffer[MAX_BUFFER_LEN];

    if((fd== open("./temp.txt", O_RDWR)) == -1){
        perror("open: ");
        exit(EXIT_FAILURE);
    }

    pollfds[0].fd = STDIN_FILENO;
    pollfds[0].events = POLLIN;

    while(1){
        memset(buffer, 0x00, MAX_BUFFER_LEN);
        retval = poll(pollfds, 1, -1);
        if(retval == -1){
            perror("poll : ");
            exit(EXIT_FAILURE);
        }

        printf("retval = %d \n", retval);

        if(pollfds[0].revents& POLLIN){
            m_read = read(STDIN_FILENO, buffer, MAX_BUFFER_LEN);
            printf("[read] m_read = %d\n", m_read);

            m_write = write(fd, buffer, m_read);
            printf("[wirte] m_write = %d\n", m_write);
        }
        usleep(1000);
    }
    close(fd);
    exit(EXIT_FAILURE);
}