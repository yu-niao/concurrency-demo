#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFSIZE 1024

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s, ip, port...\n", argv[0]);
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr.s_addr);

    int ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        perror("connect");
        exit(1);
    }
    else
    {
        puts("connected.........");
    }

    while(1)
    {
        char buf[BUFSIZE];
        bzero(&buf, BUFSIZE);
        fputs("Enter your message(q/Q to quit): ", stdout);
        fgets(buf, BUFSIZE - 1, stdin);

        if ((!strcmp(buf, "q\n") || (!strcmp(buf, "Q\n"))))
        {
            break;
        }

        int len = send(sockfd, buf, strlen(buf) + 1, 0);
        if (len < 0)
        {
            perror("send");
            break;
        }
        else if (len == 0)
        {
            printf("the server has been disconnected...\n");
            break;
        }
        else
        {
            bzero(&buf, BUFSIZE);
            recv(sockfd, buf, BUFSIZE - 1, 0);
            printf("the server says: %s...\n", buf);
        }
    }

    close(sockfd);

    return 0;
}