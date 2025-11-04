#include <stdio.h>
#include <unistd.h>  
#include <stdlib.h>    // exit()
#include <sys/wait.h>  // wait()

int main() {
    printf("프로그램 시작\n");

    int pid = fork(); 
    int status;

    if (pid == 0){
        printf("자식 프로세스입니다. PID: %d\n", getpid());
        sleep(1);
        printf("자식 프로세스 종료\n");
        exit(0);
    }
    else{
        printf("부모 프로세스입니다. 자식 PID: %d\n", pid);
        printf("부모 프로세스 대기...\n");
        wait(&status);
        printf("부모 프로세스 재시작...\n");
    }
        
    
    return 0;
}