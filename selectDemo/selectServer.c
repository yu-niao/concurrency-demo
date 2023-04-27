#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s, port.\n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        perror("socket");
        exit(1);
    }

    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        perror("bind");
        exit(1);
    }

    ret = listen(serv_sock, 5);
    if (ret == -1)
    {
        printf("listen");
        exit(1);
    }

    int maxfd = serv_sock;
    fd_set redfd;
    fd_set redtmp;
    FD_ZERO(&redfd);
    FD_SET(serv_sock, &redfd);

    while(1)
    {
        redtmp = redfd;
        int num = select(maxfd + 1, &redtmp, NULL, NULL, NULL);
        if (num == -1)
        {
            perror("select");
            exit(1);
        }

        for (int i = 0; i <= maxfd; i++)
        {
            if ((i == serv_sock) && FD_ISSET(i, &redtmp))
            {
                struct sockaddr_in clit_addr;
                socklen_t clit_len = sizeof(clit_addr);
                int confd = accept(i, (struct sockaddr*)&clit_addr, &clit_len);
                if (confd == -1)
                {
                    perror("accept");
                    exit(1);
                }

                char ip[24];
                printf("a connection has been established, and the ip is %s, the port is %d.\n",
                inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(clit_addr.sin_port));

                FD_SET(confd, &redfd);
                maxfd = (maxfd > confd ? maxfd : confd);
            }
            else if (FD_ISSET(i, &redtmp))
            {
                char buf[BUFSIZE];
                memset(&buf, 0, BUFSIZE);
                int len = recv(i, buf, BUFSIZE, 0);
                if (len > 0)
                {
                    printf("the client says: %s\n", buf);
                    send(i, "i got it!", 10, 0);
                }
                else if (len == 0)
                {
                    printf("the client has been disconnected...\n");
                    FD_CLR(i, &redfd);
                    close(i);
                    continue;
                }
                else 
                {
                    perror("recv");
                    continue;
                }
            }
        }
    }

    close(serv_sock);

    return 0;
}