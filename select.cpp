#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#define BUFSIZE 30

int main(int argc, char* argv[]){
    fd_set reads, temps;
    int result;

    char message[BUFSIZE];
    int str_len;
    struct timeval timeout;

    FD_ZERO(&reads);    //0으로 초기화
    FD_SET(0,&reads);   //파일 디스크립터 0(stdin) 설정

    /*
    timeout.tv_sec=5;
    timeout.tv_usec = 100000;
    */
   //잘못된 timeout 설정(구조체 내에서만 업데이트 된다)

   while(1){
       temps=reads;
       timeout.tv_sec = 5;  //실행후 다시 재설정 해야한ㄴ다.
       timeout.tv_usec = 0;

       result = select(1, &temps, 0,0,&timeout);
       if(result == -1){
           puts("select(): 오류 발샹");
           exit(1);
       }else if (result == 0){
           puts("select(): 시간이 초과 되었습니다");
       }else{
           if(FD_ISSET(0, &temps)){
               str_len = read(0, message, BUFSIZE);
               message[str_len]=0;
               fputs(message, stdout);
           }
       }
   }
   
   return 0;
}