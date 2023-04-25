#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <errno.h>

#define BUFSIZE 5
#define LISTENNUM 5
#define EPOLLNUM 1024

int setnonblock(int fd)
{
    int old_opt = fcntl(fd, F_GETFL);
    int new_opt = old_opt | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_opt);
    return old_opt; 
}

void addfd(int epollfd, int fd, bool enable_et)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et)
    {
        ev.events |= EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setnonblock(fd);
}

void lt(int epollfd, int num, int fd, struct epoll_event* evs)
{
    char buf[BUFSIZE];
    for (int i = 0; i < num; i++)
    {
        int sockfd = evs[i].data.fd;
        if (sockfd == fd)
        {
            struct sockaddr_in clit_addr;
            socklen_t clit_len = sizeof(clit_addr);
            int ret = accept(sockfd, (struct sockaddr*)&clit_addr, &clit_len);
            if (ret == -1)
            {
                perror("epoll-lt-accept");
                exit(1);
            }

            char ip[30] = {0};
            printf("a connection has been established, ip: %s, port: %d.\n", 
            inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(clit_addr.sin_port));

            addfd(epollfd, ret, false);
        }
        else if (evs[i].events && EPOLLIN)
        {
            bzero(&buf, BUFSIZE);

            int len = recv(sockfd, buf, BUFSIZE - 1, 0);
            if (len < 0)
            {
                perror("recv");
                continue;
            }
            else if (len == 0)
            {
                printf("the client has been disconnected...\n");
                epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL);
                close(sockfd);
            }
            else
            {
                printf("the client says: %s\n", buf);
                send(sockfd, "i get it!", 20, 0);
            }
        }
        else
        {
            printf("some else will happend...\n");
        }
    }
}

void et(int epollfd, int num, int fd, struct epoll_event* evs)
{
    char buf[BUFSIZE];
    for (int i = 0; i< num; i++)
    {
        int sockfd = evs[i].data.fd;
        if (sockfd == fd)
        {
            struct sockaddr_in clit_addr;
            socklen_t clit_len = sizeof(clit_addr);
            int ret = accept(sockfd, (struct sockaddr*)&clit_addr, &clit_len);
            if (ret == -1)
            {
                perror("epoll-et-accept");
                exit(1);
            }

            char ip[30] = {0};
            printf("a connection has been established, ip: %s, port: %d.\n", 
            inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(clit_addr.sin_port));

            addfd(epollfd, ret, true);
        }
        else if (evs[i].events && EPOLLIN)
        {
            while(1)
            {
                bzero(&buf, BUFSIZE);
                int len = recv(sockfd, buf, BUFSIZE - 1, 0);
                if (len < 0)
                {
                    if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                    {
                        printf("read later\n");
                        break;
                    }
                    perror("recv");
                    exit(1);
                }
                else if (len == 0)
                {
                    printf("the client has been disconnected...\n");
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL);
                    close(sockfd);
                    break;                    
                }
                else
                {
                    printf("the client says: %s\n", buf);
                    send(sockfd, "i get it!", 20, 0);                  
                }
            }
        }
        else
        {
            printf("something else will happend...\n");
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s, port.\n", argv[0]);
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }

    int port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        perror("bind");
        exit(1);
    }

    ret = listen(sockfd, LISTENNUM);
    if (ret == -1)
    {
        perror("listen");
        exit(1);
    }

    struct epoll_event evs[EPOLLNUM];
    
    int epollfd = epoll_create(LISTENNUM);
    if (epollfd == -1)
    {
        perror("epoll_create");
        exit(1);
    }
    addfd(epollfd, sockfd, true);

    while(1)
    {
        int num = epoll_wait(epollfd, evs, EPOLLNUM, -1);
        if (num == -1)
        {
            perror("epoll_wait");
            break;
        }

        //lt(epollfd, num, sockfd, evs);
        et(epollfd, num, sockfd, evs);
    }

    close(epollfd);
    close(sockfd);

    return 0;
}