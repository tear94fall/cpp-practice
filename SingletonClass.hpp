#ifndef SINGLETONCLASS_HPP__
#define SINFLETONCLASS_HPP__

#include <iostream>

class SingletonClass
{
  private:
    /**
   * 생성자
   * 외부에서 인스턴스를 생성할수 없도록 private 영역에서 정의하고 구현함
   * 원리 : private 영역에서 생성자가 존재하므로 외부에서 SingletonClass()를 호출 할수 없음
   */
    SingletonClass(){};

    // 싱글턴 인스턴스가 생성되었는지 여부
    static bool instanceFlag;

    // 싱글턴 인스턴스
    static SingletonClass *instance;

    // private 멤버변수(멤버 변수)
    int total;

  public:

    // 싱글턴 인스턴스를 반환할 멤버 함수
    static SingletonClass* getInstance();

    // 소멸자, instanceFlag를 false로 변경
    virtual ~SingletonClass(){
        instanceFlag = false;
    }

    // value 의 값을 증가 시킴
    void addValue(int value);

    // value의 값을 반환
    int getTotalValue();
};

#endif /* SINGLETONCLASS_HPP__ */