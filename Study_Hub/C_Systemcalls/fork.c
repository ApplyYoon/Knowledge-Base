#include <stdio.h>
#include <unistd.h>  

int main() {
    printf("프로그램 시작\n");

    int pid = fork(); 
    int status;

    if (pid == 0)
        printf("자식 프로세스입니다. PID: %d\n", getpid());
    else
        printf("부모 프로세스입니다. 자식 PID: %d\n", pid);
    
    printf("공통 코드 실행 중...\n");
    return 0;
}