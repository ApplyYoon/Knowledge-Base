#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    printf("소켓 생성 성공! fd 번호: %d\n", sockfd);
    
    close(sockfd);
    return 0;
}