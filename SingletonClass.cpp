#include "SingletonClass.hpp"

bool SingletonClass::instanceFlag = false;
SingletonClass* SingletonClass::instance = NULL;

// 싱글턴 인스턴스를 반환할 멤버 함수
SingletonClass* SingletonClass::getInstance(){
    if(!instance){
        instance=new SingletonClass();
        instanceFlag = true;
    }
}

// total의 값을 증가 시킴
void SingletonClass::addValue(int value){
    total = total + value;
}

// total 값을 반환
int SingletonClass::getTotalValue(){
    return total;
}