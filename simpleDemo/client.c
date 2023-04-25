#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFSIZE 1024
int num = 0;

int main(int argc, char* argv[])
{
    int lfd;

    // if (argc != 3)
    // {
    //     printf("Usage: %s, <ip>, <port>.\n", argv[0]);
    //     exit(1);
    // }

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        exit(1);
    }

    // const char* ip = argv[1];
    // int port = atoi(argv[2]);
    struct sockaddr_in caddr;
    caddr.sin_family = AF_INET;
    caddr.sin_port = htons(12345);
    inet_pton(AF_INET, "192.168.88.100", &caddr.sin_addr.s_addr);

    int ret = connect(lfd, (struct sockaddr*)&caddr, sizeof(caddr));
    if (ret == -1)
    {
        perror("connect");
        exit(1);
    }
    else
    {
        puts("connected............");
    }

    char buf[BUFSIZE];
    bzero(buf, BUFSIZE);
    while(1)
    {
        fputs("input buf(Q/q to quit): ", stdout);
        fgets(buf, BUFSIZE, stdin);

        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
            break;

        int len = send(lfd, buf, strlen(buf) + 1, 0);
        if (len == -1)
        {
            perror("send");
            exit(1);
        }

        bzero(buf, BUFSIZE);
        len = recv(lfd, buf, BUFSIZE, 0);
        if (len > 0)
        {
            printf("server says: %s\n", buf);
        }
        else if (len == 0)
        {
            printf("the server has been disconnected....\n");
            break;
        }
        else 
        {
            perror("recv");
            break;
        }

        sleep(1);
    }

    close(lfd);

    return 0;
}