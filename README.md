# 리눅스 I/O 멀티 플렉싱
#### 헤더 선언방법
`#include <sys/epoll.h>`
#### epoll_create 사용법
`int epoll_create(int size);`

반환값은 다음과 같다.
실패시 **-1** 을 반환한다.
일반적으로는 epoll 인스턴스를 생성하고, 이 인스턴스를 참조하는 fd를 리턴한다.

size크기의 epoll 인스턴스를 생성하는 함수
하지만 size는 참고로만 사용되면 실제로는 운영체제가 적당한 크기로 생성한다.

#### epoll_ctl 사용법
`int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);`

**op**파라미터 에는 다음을 사용할 수 있다.

* EPOLL_CTL_ADD
* EPOLL_CTL_MOD
* EPOLL_CTL_DEL

#### FD_SET 구조체
FD_SET은 하나의 파일 디스크립터(File Descriptor)의 상태로 하나의 **비트**를 표현한다.
 
