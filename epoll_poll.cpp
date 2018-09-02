#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <pthread.h>

const int EPOLL_SIZE = 5000;
const int EVENT_ARR = 5000;
const int PORT = 8002;
const int BUF_SIZE = 5000;
const int BACK_QUEUE = 100;
const int THREAD_MAX = 100;

static unsigned int s_thread_para[THREAD_MAX][8];
static pthread_t s_tid[THREAD_MAX];
pthread_mutex_t s_mutex[THREAD_MAX];
int epFd;
struct epoll_event ev,evs[EVENT_ARR];

char *get_type(char *url, char *buf){
    const char *t = url+strlen(url);
    char type[64];
    for(;t>=url&&*t!='.';--t){
        ;
    }

    strcpy(type, t+1);
    if(strcmp(type,"html")==0||strcmp(type,"htm")==0){
        sprintf(buf,"text/%s",type);
    }else if(strcmp(type, "gif")==0||
            strcmp(type, "jpg")==0||
            strcmp(type, "jpeg")==0||
            strcmp(type, "png")==0){
            sprintf(buf, "image/%s",type);
    }else if(strcmp(type,"/")==0){
        sprintf(buf, "text/html");
    }else if(strcmp(type,"css")==0){
        sprintf(buf,"text/css");
    }else if(strcmp(type,"js")==0){
        sprintf(buf,"application/x-javascript");
    }else{
        sprintf(buf, "unknown");
    }
    return buf;
}

void* http_server(int thread_para[]){
    int poll_index;
    int clientFd;
    char buf[BUF_SIZE];
    pthread_detach(pthread_self());
    poll_index=thread_para[7];

    wait_unlock:
    pthread_mutex_lock(s_mutex+poll_index);
    clientFd=thread_para[1];

    int len = read(clientFd, buf, BUF_SIZE);
    printf("%s",buf);
    if(len>0){
        char *token=strtok(buf," ");
        printf("token: %s", token);
        char type[64];
        char *url=strtok(NULL," ");
        while(*url=='.'||*url=='/'){
            ++url;
        }
        printf("url: %s",url);
        char file[1280000];

        sprintf(file, "%s", url);
        printf("file:%s", file);

        FILE *fp=fopen(file, "rb");

        if(fp==0){
            char response[]="HTTP/1.1 404 NOT FOUND\r\n\r\n";
            printf("HTTP/1.1 404 NOT FOUND\r\n\r\n");
            write(clientFd, response, strlen(response));
        }else{
            int file_size;
            char *content;
            char *response;
            fseek(fp, 0, SEEK_END);
            file_size = ftell(fp);
            fseek(fp,0,SEEK_SET);

            content = (char*)malloc(file_size+1);
            response = (char*)malloc(200);
            fread(content, file_size, 1, fp);
            content[file_size]=0;
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type:%s\r\n\r\n",file_size, get_type(url, type));
            write(clientFd, response, strlen(response));
            write(clientFd,content,file_size);
            free(content);
            free(response);
        }
    }else if(len==0){
        epoll_ctl(epFd,EPOLL_CTL_DEL, clientFd,&ev);
        close(clientFd);
        int i=thread_para[3];
        evs[i].data.fd=-1;
    }else if(len==EAGAIN){
        printf("socket !!\n");
    }else{
        printf("CLient read failed\n");
    }
    thread_para[0]=0;
    goto wait_unlock;

    printf("pthread eixt!\n");
    pthread_exit(NULL);
}

static int init_thread_pool(void){
    int i,rc;
    for(i=0;i<THREAD_MAX;i++){
        s_thread_para[i][0]=0;
        s_thread_para[i][7]=i;
        pthread_mutex_lock(s_mutex+i);
    }

    for(i=0;i<THREAD_MAX;i++){
        rc=pthread_create(s_tid+i,0,(void*(*)(void*))&http_server,(void*)(s_thread_para[i]));
        if(0!=rc){
            fprintf(stderr, "Create thread failed!\n");
            return -1;
        }
    }
    return 0;
}

void setnoblock(int sockFd){
    int opt;
    if((opt=fcntl(sockFd,F_GETFL))<0){
        printf("GET FL failed!\n");
        exit(-1);
    }
    opt|=O_NONBLOCK;
    if(fcntl(sockFd, F_SETFL, opt)<0){
        printf("SET FL failed!\n");
    }
}

int main(int argc, char* argv[]){
    int serverFd,j;

    unsigned int optval;
    struct linger optval1;

    optval = 0x1;
    setsockopt(serverFd, SOL_SOCKET, SO_LINGER, &optval, 4);

    optval1.l_onoff=1;
    optval1.l_linger=60;

    setsockopt(serverFd, SOL_SOCKET, SO_LINGER, &optval1, sizeof(struct linger));

    epFd=epoll_create(EPOLL_SIZE);

    ev.data.fd=serverFd;
    ev.events=EPOLLIN|EPOLLET;

    epoll_ctl(epFd, EPOLL_CTL_ADD, serverFd, &ev);

    struct sockaddr_in serverAddr;
    socklen_t serverlen = sizeof(struct sockaddr_in);
    serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    serverAddr.sin_port=htons(PORT);
    if(bind(serverFd,(struct sockaddr*)&serverAddr,serverlen)){
        printf("BIND failed!\n");
        exit(-1);
    }

    int rc=init_thread_pool();
    if(0!=rc){
        exit(-1);
    }

    if(listen(serverFd, BACK_QUEUE)){
        printf("Listen failed\n");
        exit(-1);
    }

    int clientFd;
    sockaddr_in clientAddr;
    socklen_t clientlen;

    for(;;){
        int nfds=epoll_wait(epFd, evs, EVENT_ARR, -1);

        for(int i=0;i<nfds;i++){
            if(evs[i].data.fd==serverFd&&evs[i].events&EPOLLIN){
                if((clientFd=accept(serverFd,(struct sockaddr*)&clientAddr, &clientlen))<0){
                    printf("ACCEPT failed\n");
                }printf("Connect from %s:%d\n", inet_ntoa(clientAddr.sin_addr),htons(clientAddr.sin_port));
                setnoblock(clientFd);
                ev.data.fd=clientFd;
                ev.events=EPOLLIN|EPOLLET;
                epoll_ctl(epFd,EPOLL_CTL_ADD,clientFd,&ev);
            }else if(evs[i].events&&EPOLLIN){
                printf("Client can write!\n");
                if((clientFd=evs[i].data.fd)>0){
                    for(j=0;j<THREAD_MAX;j++){
                        if(0==s_thread_para[j][0]){
                            break;
                        }
                    }
                    if(j>=THREAD_MAX){
                        fprintf(stderr, "thread is full\n");
                        shutdown(clientFd, SHUT_RDWR);
                        close(clientFd);
                        continue;
                    }

                    s_thread_para[j][0]=1;
                    s_thread_para[j][1]=clientFd;
                    s_thread_para[j][2]=serverFd;
                    s_thread_para[j][3]=i;

                    pthread_mutex_unlock(s_mutex+j);
                }else{
                    printf("othrer error!\n");
                }
            }
        }
    }
    return 0;
}