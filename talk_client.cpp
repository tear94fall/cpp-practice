#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 1024

const char *escapechar = "exit";
void z_handler();

int main(int argc,char* argv[]){
    char line[MAXLINE], sendline[MAXLINE], recvline[MAXLINE+1];
    int n, size, comp, addr_size, state;
    pid_t fork_ret;

    static int s;
    static struct sockaddr_in server_addr;

    if(argc!=3){
        printf("Usage: %s <Host><Port>\n", argv[0]);
        
        exit(0);
    }

    /* 소켓 생성 */
    if((s=socket(PF_INET, SOCK_STREAM,0))<0){
        printf("CLient: can't open stream socket.\n");
        exit(0);
    }
    
    /* 소켓주소 구조체에 접속할 서버 주소 세팅 */
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port =  htons(atoi(argv[2]));

    /* 서버에 연결 요청 */
    if(connect(s, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Client: can't connect to server \n");
        exit(0);
    }

    fork_ret = fork();

    if(fork_ret == 0){
        /* 자식 프로세스는 키보드 입력을 서버로 송신 */
        while(fgets(sendline, MAXLINE, stdin) != NULL){
            size = strlen(sendline);
            if(write(s, sendline, strlen(sendline))!=size){
                printf("Error in write. \n");
            }
            /* 종료 문자열 입력시 처리 */
            if(strstr(sendline, escapechar)!=NULL){
                printf("Good bye.\n");
                close(s);
                exit(0);
            }
        }
    }else if(fork_ret > 0){
        /* 부모 프로세스는 서버로 부터 수신된 메세지를 화면에 출력 */
        while(1){
            if((size = read(s, recvline, MAXLINE))<0){
                printf("Error if read \n");
                close(s);
                exit(0);
            }
            recvline[size] = '\0';
            /* 종료 문자열 입력시 처리 */
            if(strstr(recvline, escapechar)!=NULL){
                break;

                printf("%s", recvline);
            }
        }
    }
    close(s);
    return 0;
}