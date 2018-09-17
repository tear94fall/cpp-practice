#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERV_PORT 8787
#define MAX_THREAD_CNT 100
#define MAX_CLI_CNT 10
#define MAX_BUFF_SZ 1024

#define UNUSED 0
#define USED 1

typedef struct
{
    int sockfd;
    int used;
    int state;
    char recv_buff[1024];
} cli_pool_t;
cli_pool_t cli_pool[MAX_CLI_CNT];

fd_set rset, allset;

int DelClientPool(int idx);
int InitClientPool(int sockfd);
int AddClientPool(int sockfd);
int CreateTCPServerSocket(unsigned short port);

main()
{
    char cliaddr_buff[100];
    int cli_fd, sock_fd;
    socklen_t clilen;
    int i, result, maxfd, s_cnt, idx;
    int serv_port, thread_cnt;
    char recv_buff[MAX_BUFF_SZ];
    struct sockaddr_in cli_addr;
    struct epoll_event *event;
    struct epoll_event ev;
    int efd;

    serv_port = SERV_PORT;
    sock_fd = CreateTCPServerSocket(serv_port);
    if (sock_fd < 0)
    {
        perror("server socket failed.");
        exit(1);
    }

    thread_cnt = MAX_THREAD_CNT;

    efd = epoll_create(MAX_CLI_CNT);
    if (efd < 0)
    {
        perror("epoll create error.");
        exit(2);
    }

    event = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAX_CLI_CNT);

    result = InitClientPool(sock_fd);
    ev.events = EPOLLIN;
    ev.data.fd = result;
    epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &ev);

    clilen = sizeof(cli_addr);

    for (;;)
    {
        s_cnt = epoll_wait(efd, event, MAX_CLI_CNT, -1);
        if (s_cnt <= 0)
        {
            perror("epoll_wait fail.");
            continue;
        }

        for (i = 0; i < s_cnt; i++)
        {
            if (event[i].data.fd == 0)
            { // Server sock
                cli_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &clilen);
                if (cli_fd <= 0)
                {
                    perror("accept fail.");
                }
                else if (cli_fd > 0)
                {
                    result = AddClientPool(cli_fd);
                    if (result < 0)
                    {
                        perror("AddClientPool Error");
                        close(cli_fd);
                    }
                    else
                    {
                        ev.events = EPOLLIN;
                        ev.data.fd = result;
                        result = epoll_ctl(efd, EPOLL_CTL_ADD, cli_fd, &ev);
                    }
                }
                continue;
            }
            else
            {
                idx = event[i].data.fd;
                printf("sockfd(%d) idx(%d), s_cnt(%d)\n", cli_pool[idx].sockfd, idx, s_cnt);
                memset(cli_pool[idx].recv_buff, 0x00, sizeof(cli_pool[idx].recv_buff));
                result = read(cli_pool[idx].sockfd, cli_pool[idx].recv_buff, sizeof(cli_pool[idx].recv_buff));
                if (result < 0)
                {
                    printf("ERR : read fail (%d)\n", result);
                    epoll_ctl(efd, EPOLL_CTL_DEL, cli_pool[idx].sockfd, event);
                    DelClientPool(idx);
                }
                else if (result == 0)
                {
                    printf("ERR : client disconnect idx(%d)\n", idx);
                    epoll_ctl(efd, EPOLL_CTL_DEL, cli_pool[idx].sockfd, event);
                    DelClientPool(idx);
                }
                else
                {
                    for (i = 1; i <= MAX_CLI_CNT; i++)
                    {
                        if (cli_pool[i].state == USED)
                            send(cli_pool[i].sockfd, cli_pool[idx].recv_buff, strlen(cli_pool[idx].recv_buff), 0);
                    }
                    printf("%s\n", cli_pool[idx].recv_buff);
                }
            }
        }
    }

    return 0;
}

int InitClientPool(int sockfd)
{
    int i;
    for (i = 0; i < MAX_CLI_CNT; i++)
    {
        cli_pool[i].sockfd = -1;
        cli_pool[i].state == UNUSED;
    }
    cli_pool[0].sockfd = sockfd;
    cli_pool[0].state == USED;

    return 0;
}

int AddClientPool(int sockfd)
{
    int i;
    for (i = 1; i < MAX_CLI_CNT; i++)
    {
        if (cli_pool[i].state == UNUSED)
        {
            cli_pool[i].sockfd = sockfd;
            cli_pool[i].state = USED;
            return i;
        }
    }

    return -1;
}

int DelClientPool(int idx)
{
    close(cli_pool[idx].sockfd);
    cli_pool[idx].state = UNUSED;
    memset(cli_pool[idx].recv_buff, 0x00, sizeof(cli_pool[idx].recv_buff));
}

int CreateTCPServerSocket(unsigned short port)
{
    int sock;
    struct sockaddr_in servaddr;

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("socket() failed.\n");
        return -1;
    }

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //  servaddr.sin_addr.s_addr = htonl(inet_addr("127.0.0.1"));
    servaddr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0)
    {
        printf("Bind() failed.\n");
        return -2;
    }

    if (listen(sock, SOMAXCONN))
    {
        printf("Listen() failed.\n");
        return -3;
    }

    return sock;
}