#include <sys/poll.h>
#include <stdio.h>
#include <iostream>
#include <string>

template<typename T1, typename T2>
struct test_struct{
    T1 num;
    T2 name;
};

int main(void){
    test_struct<int, std::string> info;



}