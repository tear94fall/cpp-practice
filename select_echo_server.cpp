
/*
 server
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <string.h>

#define FD_MAX_SIZE 1024

std::string curr_time(void);

int main(int argc, char *argv[]){
     int server_sockfd, client_sockfd, sockfd;
     int state;
     int i, max_client, maxfd;
     int client[FD_MAX_SIZE];
     
     socklen_t client_len;
     
     struct sockaddr_in clientaddr, serveraddr;
     fd_set readfds, allfds;
     
     int current_cli_num;
     char buf[255];
     char line[255];
     
     memset(line, 0x00, 255);
     state = 0;
     current_cli_num = 3;
     
     if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
          perror("socket error: ");
          exit(0);
     }
     
     memset(&serveraddr, 0x00, sizeof(serveraddr));
     serveraddr.sin_family = AF_INET;
     serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
     serveraddr.sin_port = htons(9999);
     
     state = bind(server_sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
     
     if(state == -1){
          perror("bind error: ");
          exit(0);
     }
     
     state = listen(server_sockfd, 5);
     
     if(state==-1){
          perror("listen error: ");
          exit(0);
     }
     
     client_sockfd = server_sockfd;
     
     max_client = -1;
     maxfd = server_sockfd;
     
     for(i=0;i<FD_MAX_SIZE;i++){
          client[i] = -1;
     }
     
     FD_ZERO(&readfds);
     FD_SET(server_sockfd, &readfds);
     
     printf("\n[%s] TCP Server Start...\n", curr_time().c_str());
     fflush(stdout);
     
     while(1){
          allfds = readfds;
          
          state = select(maxfd+1, &allfds, NULL, NULL, NULL);
          
          if(FD_ISSET(server_sockfd, &allfds)){
               client_len = sizeof(clientaddr);
               client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr, &client_len);
               printf("[%s] connection from (%s, %d)\n", curr_time().c_str(),inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
               
               for(i=0;i<FD_MAX_SIZE;i++){
                    if(client[i]<0){
                         client[i] = client_sockfd;
                         printf("[%s] clientNUM=%d clientFD=%D ", curr_time().c_str(), i+1, client_sockfd);
                         break;
                    }
               }
               
               printf("accept [%d]\n", client_sockfd);
               
               if(i==FD_MAX_SIZE){
                    perror("too many clients\n");
               }
               
               FD_SET(client_sockfd, &readfds);
               
               if(client_sockfd>maxfd){
                    maxfd = client_sockfd;
               }
               
               if(i>max_client){
                    max_client = i;
               }
               
               if(--state <= 0){
                    continue;
               }
          }
          
          for(i=0;i<max_client+1;i++){
               if((sockfd=client[i])<0){
                    continue;
               }
               
               if(FD_ISSET(sockfd, &allfds)){
                    memset(buf, 0x00, 255);
                    
                    if(read(sockfd, buf, 255)<=0){
                         close(sockfd);
                         perror("Close sockfd ");
                         FD_CLR(sockfd, &readfds);
                         client[i]=-1;
                    }
                    
                    if(strlen(buf)!=0){
                         printf("[%s] [%d] RECV: %s", curr_time().c_str(), sockfd, buf);
                         write(sockfd, buf, 255);
                         printf("[%s] [%d] SEND: %s", curr_time().c_str(), sockfd, buf);
                    }
                    if(--state<=0){
                         break;
                    }
               }
          }
     }
     
     return 0;
}

std::string curr_time(void){
     time_t curr_time;
     
     struct tm *curr_tm;
     
     curr_time = time(NULL);
     
     curr_tm = localtime(&curr_time);
     
     std::string current_yyyy_mm_dd_str;
     std::string current_hh_mm_ss_str;
     std::string complete_curr_time_str;
     
     current_yyyy_mm_dd_str = std::to_string(curr_tm->tm_year + 1900) + "-" +
     std::to_string(curr_tm->tm_mon + 1)+"-"+
     std::to_string(curr_tm->tm_mday);
     
     current_hh_mm_ss_str = std::to_string(curr_tm->tm_hour)+":"+
     std::to_string(curr_tm->tm_min)+":"+
     std::to_string(curr_tm->tm_sec);
     
     complete_curr_time_str = current_yyyy_mm_dd_str + " " + current_hh_mm_ss_str;
     
     return complete_curr_time_str;
}
