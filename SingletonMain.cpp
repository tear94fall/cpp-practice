#include <iostream>
#include "SingletonClass.hpp"

int main(int argc, char* argv[]){
    std::cout<<"SingletonMain"<<std::endl;

    // SingletonClass 의 인스턴스 변수를 포인터 변수 형태로 선언
    SingletonClass *ins1, *ins2, *ins3;

    // 각각의 변수에 인스턴스를 저장
    ins1 = SingletonClass::getInstance();
    ins2 = SingletonClass::getInstance();
    ins3 = SingletonClass::getInstance();

    ins1->addValue(10);
    std::cout<<"total: "<<ins1->getTotalValue()<<std::endl;
    ins2->addValue(20);
    std::cout<<"total: "<<ins2->getTotalValue()<<std::endl;
    ins3->addValue(30);
    std::cout<<"total: "<<ins3->getTotalValue()<<std::endl;

    return 0;
}