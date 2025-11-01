#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void) {
    int sockfd;
    struct  sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }
    
    // 주소 구조체 초기화
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;                 // IPV4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // Any IP
    servaddr.sin_port = htons(8080);               // Port: 8080

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind error");
        exit(1);
    }

    // listen
    // netstat -an | grep 8080
    if (listen(sockfd, 5) < 0) {
        perror("listen error");
        exit(1);
    }

    printf("소켓 바인드 성공 and Listen ... \n");
    
    sleep(10);

    close(sockfd);
    return 0;
}
