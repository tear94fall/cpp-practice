#include <iostream>
#include <thread>

void proc(int no);

int main(void){
    std::thread thread1(proc,1);
    std::thread thread2(proc,2);

    thread1.join();
    thread2.join();

    return 0;
}

void proc(int no){
    for(int i=0;i<10;i++){
        std::cout<<"Thread#: "<<no<<" /count: "<<i<<std::endl;
    }
}