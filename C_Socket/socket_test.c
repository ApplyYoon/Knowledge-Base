#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void) {
    // 소켓 생성(return -> fd)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 오류 검사
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    printf("소켓 생성 성공! fd 번호: %d\n", sockfd);

    // 잠깐 대기해서 확인할 수 있게
    printf("잠시 대기 중... (Ctrl+C로 종료)\n");
    sleep(10);

    // 소켓 종료
    close(sockfd);
    return 0;
}