#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENT 10000
#define DEFAULT_PORT 9006
#define MAX_EVENTS 10000

int g_svr_sockfd;
int g_svr_port;

struct
{
    int cli_sockfd;
    char cli_ip[20];
} g_client[MAX_CLIENT];

int g_epoll_fd;

struct epoll_event g_event[MAX_EVENTS];

void init_data0(void);
void init_server0(int svr_port);
void epoll_init(void);
void epoll_cli_add(int cli_fd);
void userpool_add(int cli_fd, char *cli_ip);

/*----------------------------------------------------------*/
/*  FUNCTION PART
------------------------------------------------------------*/

/*----------------------------------------------------------
    function: init_data0
    io: none
    desc: initialize global client structure value
------------------------------------------------------------*/

void init_data0(void)
{
    register int i;

    for (i = 0; i < MAX_CLIENT; i++)
    {
        g_client[i].cli_sockfd = -1;
    }
}

/*-----------------------------------------------------------
    function: init_server0
    io: input: integer - server port(must be positive)
    output: none
    desc: tcp/ip listening socket setting with input variable
    ----------------------------------------------------------*/

void init_server0(int svr_port)
{
    struct sockaddr_in serv_addr;

    /* Open TCP Socket */
    if ((g_svr_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("[ETEST] SErver Start Fails: Can't open stream socket \n");
        exit(0);
    }

    /* Address Setting */
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(svr_port);

    /* Set Socket Option */
    int nSocketOpt = 1;
    if (setsockopt(g_svr_sockfd, SOL_SOCKET, SO_REUSEADDR, &nSocketOpt, sizeof(nSocketOpt)) < 0)
    {
        printf("[ETEST] Server Start Fails: Can't set reuse address\n");
        close(g_svr_sockfd);
        exit(0);
    }

    /* Bind Socket */
    if (bind(g_svr_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("[ETEST] Server Start Fails: Can't bind local address\n");
        close(g_svr_sockfd);
        exit(0);
    }

    /* Listening */
    listen(g_svr_sockfd, 15); /* connection queue is 15 */

    printf("[ETEST][START] Now Server listening on port %d\n", svr_port);
}
/*-------------------------------------end of function init_server0 */

void epoll_init(void)
{
    struct epoll_event events;

    g_epoll_fd = epoll_create(MAX_EVENTS);
    if (g_epoll_fd < 0)
    {
        printf("[ETEST] Epoll create Fails\n");
        close(g_svr_sockfd);
        exit(0);
    }

    printf("[ETEST][START] epoll creation success\n");

    /* event control set */
    events.events = EPOLLIN;
    events.data.fd = g_svr_sockfd;

    /* server events set(read for accept) */
    if (epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_svr_sockfd, &events) < 0)
    {
        printf("[ETEST] Epoll control fails\n");
        close(g_svr_sockfd);
        close(g_epoll_fd);
        exit(0);
    }
    printf("[ETEST][START] epoll events set success for server\n");
}

void epoll_cli_add(int cli_fd){
    struct epoll_event events;

    /* event control set for read event */
    events.events = EPOLLIN;
    events.data.fd = cli_fd;

    if(epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, cli_fd, &events)<0){
        printf("[ETEST] Epoll control fails. in epoll_cli_add\n");
    }
}

void userpool_add(int cli_fd, char* cli_ip){
    /*get empty element */
    register int i;

    for(i =0;i<MAX_CLIENT;i++){
        if(g_client[i].cli_sockfd == -1){
            break;
        }
    }
    if(i>=MAX_CLIENT) close(cli_fd);

    g_client[i].cli_sockfd = cli_fd;
    memset(&g_client[i].cli_ip[0],0,20);
    strcpy(&g_client[i].cli_ip[0],cli_ip);
}

void userpool_delete(int cli_fd){
    register int i;

    for(i=0;i<MAX_CLIENT;i++){
        if(g_client[i].cli_sockfd == cli_fd){
            g_client[i].cli_sockfd = -1;
            break;
        }
    }
}

void userpool_send(char* buffer){
    register int i;
    int len;

    len = strlen(buffer);
    for(i=0;i<MAX_CLIENT;i++){
        if(g_client[i].cli_sockfd != -1){
            len = send(g_client[i].cli_sockfd, buffer, len, 0);
            /* more precise code needed here */
        }
    }
}

void client_recv(int event_fd){
    char r_buffer[1024];
    int len;

    len = recv(event_fd, r_buffer, 1024, 0);
    if(len<0 || len == 0){
        userpool_delete(event_fd);
        close(event_fd);
        return;
    }
}

void server_process(void){
    struct sockaddr_in cli_addr;
    int i,nfds;
    int cli_sockfd;
    int cli_len = sizeof(cli_addr);

    nfds=epoll_wait(g_epoll_fd, g_event, MAX_EVENTS, 100);

    if(nfds == 0){
        return;
    }
    if(nfds<0){
        printf("[ETEST] epoll wait error\n");
        return;
    }

    for(i=0;i<nfds;i++){
        if(g_event[i].data.fd == g_svr_sockfd){
            cli_sockfd = accept(g_svr_sockfd, (struct sockaddr*)& cli_addr,(socklen_t*)&cli_len);

            if(cli_sockfd<0){
            }else{
                printf("[ETEST][Accept] New client connected. fd: %d, ip: %s\n", cli_sockfd, inet_ntoa(cli_addr.sin_addr));
                userpool_add(cli_sockfd, inet_ntoa(cli_addr.sin_addr));
                epoll_cli_add(cli_sockfd);
            }
            continue;
        }
        client_recv(g_event[i].data.fd);
    }
}

/* -------------------------------------- end of function server_process */

void end_server(int sig){
    close(g_svr_sockfd);
    printf("[ETEST][SHUTDOWN] Server closed by signal %d\n", sig);
    exit(0);
}

int main(int argc,char* argv[]){
    printf("[ETEST][START] epoll test server v1.0(simple epoll test server)\n");
    if(argc<3){
        g_svr_port = DEFAULT_PORT;
    }else{
        if(strcmp("--port", argv[1])==0){
            g_svr_port = atoi(argv[2]);
            if(g_svr_port<1024){
                printf("[ETEST][STOP] port number iinvalid: %d\n", g_svr_port);
                exit(0);
            }
        }
    }

    init_data0();

    init_server0(g_svr_port);
    epoll_init();

    while(1){
        server_process();
    }
    
    return 0;
}