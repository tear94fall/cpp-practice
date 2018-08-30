#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>

void *task1(void*);

static int connFd;

int main(int argc, char* argv[]){
    int pId, portNo, listenFd;
    bool loop = false;
    struct sockaddr_in svrAdd,clntAdd;

    pthread_t threadA[3];

    if(argc<2){
        std::cerr<<"Syntam : ./server <port>"<<std::endl;
        return 0;
    }

    portNo=atoi(argv[1]);

    if((portNo>65535)||(portNo<2000)){
        std::cerr<<"Please enter a port number between 2000-65535"<<std::endl;
        return 0;
    }

    listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(listenFd<0){
        std::cerr<<"Cannot open socket"<<std::endl;
        return 0;
    }

    bzero((char*) &svrAdd, sizeof(svrAdd));

    svrAdd.sin_family=AF_INET;
    svrAdd.sin_addr.s_addr=INADDR_ANY;
    svrAdd.sin_port=htons(portNo);

    if(bind(listenFd,(struct sockaddr*)&svrAdd, sizeof(svrAdd))<0){
        std::cerr<<"Cannot bind"<<std::endl;
        return 0;
    }

    listen(listenFd, 5);

    int noThread=0;

    while(noThread<3){
        socklen_t len=sizeof(clntAdd);

        std::cout<<"Listening"<<std::endl;

        connFd=accept(listenFd, (struct sockaddr*)&clntAdd, &len);

        if(connFd<0){
            std::cerr<<"Cannot accept connection"<<std::endl;
            return 0;
        }else{
            std::cout<<"Connection successful"<<std::endl;
        }

        pthread_create(&threadA[noThread], NULL, task1, NULL);

        noThread++;
    }

    for(int i=0;i<3;i++){
        pthread_join(threadA[i], NULL);
    }
}

void *task1(void *dummyPt){
    std::cout<<"Thread No: "<<pthread_self()<<std::endl;
    char test[300];
    bzero(test,301);

    bool loop = false;
    while(!loop){
        bzero(test,301);

        read(connFd, test, 300);

        std::string tester(test);
        std::cout<<tester<<std::endl;

        if(test=="exit"){
            break;
        }
    }
    std::cout<<"\nClosing thrad and conn"<<std::endl;
    close(connFd);
}