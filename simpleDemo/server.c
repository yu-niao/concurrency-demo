#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFSIZE 1024

int main(int argc, char* argv[])
{
    int lfd, cfd;

    // if (argc != 2)
    // {
    //     printf("Usage: %s, <port>.\n", argv[0]);
    //     exit(1);
    // }

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(12345);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("bind");
        exit(1);
    }

    ret = listen(lfd, 5);
    if (ret == -1)
    {
        perror("listen");
        exit(1);
    }

    // printf("开始睡眠10s\n");
    // sleep(10);
    // printf("结束睡眠10s\n");   

    struct sockaddr_in caddr;
    socklen_t clen = sizeof(caddr);
    cfd = accept(lfd, (struct sockaddr*)&caddr, &clen);
    if (cfd == -1)
    {
        perror("accept");
        exit(1);
    }
    else
    {
        char ip[30] = {0};
        printf("建立一个连接, ip为: %s, 端口为: %d\n", inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(caddr.sin_port));
    }

    char buf[BUFSIZE];
    bzero(buf, BUFSIZE);
    while(1)
    {

        int len = recv(cfd, buf, BUFSIZE, 0);
        if (len > 0)
        {
            printf("the client says: %s\n", buf);
            send(cfd, buf, len, 0);
        }
        else if (len == 0)
        {
            printf("the client has been disconnected...\n");
            break;
        }
        else
        {
            perror("recv");
            break;
        }

    }

    close(lfd);
    close(cfd);

    return 0;
}
