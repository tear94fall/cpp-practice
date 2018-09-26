#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include <string.h>

#include <stdlib.h>
#include <errno.h>

extern int errno;

#define MAXBYTE 10
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 10012
#define INFTIM 1000
#define LOCAL_ADDR "127.0.0.1"
#define TIMEOUT 500