#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/tcp.h>
#include <cstring>
#include "stdlib.h"
#include <sys/types.h>
#include <net/if.h>

using namespace std;
#define MAXLINE 5
#define LISTENQ 20
#define NUM_SOCKET 1000000
#define DEFAULT_REMOTE_PORT 12345
#define DEFAULT_REMOTE_IP "127.0.0.1"
#define DEFAULT_LOCAP_IP ""
#define DEFAULT_CONNECT_COUNT 10

typedef void (*PF_CALLBACK)(int fd);
int count;

class CEpoll
{
public:
    CEpoll(PF_CALLBACK pfOnRead, PF_CALLBACK pfOnWrite, PF_CALLBACK pfOnError)
    {
        m_epoll = epoll_create(100); //
        if (m_epoll == -1)
        {
            throw "error create epoll ~";
        }

        m_OnRead = pfOnRead;
        m_OnWrite = pfOnWrite;
        m_OnError = pfOnError;
    }

    ~CEpoll()
    {
        if (m_epoll >= 0)
        {
            close(m_epoll);
        }
    }

public:
    bool AddFdOnWrite(int fd)
    {
        struct epoll_event ev;
        ev.data.fd = fd;
        //ev.data.ptr = "i am sincoder";//
        ev.events = EPOLLOUT | EPOLLET;

        int ret = epoll_ctl(m_epoll, EPOLL_CTL_ADD, fd, &ev);
        if (ret == -1)
        {
            return false;
        }
        return true;
    }
    bool AddFdOnRead(int fd)
    {

        struct epoll_event ev;
        ev.data.fd = fd;
        //ev.data.ptr = "i am sincoder";//
        ev.events = EPOLLIN | EPOLLET;

        int ret = epoll_ctl(m_epoll, EPOLL_CTL_ADD, fd, &ev);
        if (ret == -1)
        {
            return false;
        }
        return true;
    }

    bool ProcessMsg()
    {
        int i;
        int nr_events = 0;
        int ret;
        char buff[1024];
        struct epoll_event events[64];

        printf("enter wait !\n");
        nr_events = epoll_wait(m_epoll, events, 64, -1);
        printf("out wait%d\n", nr_events);

        if (nr_events < 0)
        {
            perror("epoll_wait");
            return false;
        }

        for (i = 0; i < nr_events; i++)
        {
            printf(" ProcessMsg fd=%d \n", events[i].data.fd);
            if (events[i].events & EPOLLIN)
            {
                m_OnRead(events[i].data.fd);
            }
            if (events[i].events & EPOLLOUT)
            {
                m_OnWrite(events[i].data.fd);
            }
            if (events[i].events & EPOLLERR)
            {
                m_OnError(events[i].data.fd);
            }
        }
        return true;
    }

private:
    int m_epoll;
    PF_CALLBACK m_OnRead;
    PF_CALLBACK m_OnWrite;
    PF_CALLBACK m_OnError;
};

void OnRead(int fd);
int client_socket;
CEpoll *g_epoll = NULL;

void OnRead(int fd)
{
    printf("enter call back ~\n");
    if (fd == client_socket)
    {
    }
    else
    {
        char buff[1024];
        int ret = recv(fd, buff, sizeof(buff), 0);
        if (ret <= 0)
        {
            printf("close fd %d \n", fd);
            close(fd);
        }
        else
        {
            printf("recv %d bytes from fd %d \n", ret, fd);
        }
    }
}

void OnWrite(int fd)
{
    //看看 socket 是不是链接成功了
    int result;
    socklen_t result_len = sizeof(result);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0)
    {
        printf("OnWrite socket getsockopt fail fd:%d\n", fd);
        close(fd);
        return;
    }

    if (result != 0)
    {
        printf("socket connect fail fd:%d\n", fd);
        close(fd);
        return;
    }
    ++count;
    printf("socket connect success fd:%d, count:%d\n", fd, count);
}

void OnError(int fd)
{
    if (fd == client_socket)
    {
        //connect error ~!
        printf("connect Error ~! \n");
        close(fd);
        return;
    }
}

void SetNoDelay(int iFd)
{
    int flag = 1;
    int ret = setsockopt(iFd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
    if (ret == -1)
    {
        fprintf(stderr, "Couldn't setsockopt(TCP_NODELAY)\n");
    }
}

void SetNoblock(int iFd)
{
    int iOpt = fcntl(iFd, F_GETFD, 0);
    if (iOpt == -1)
    {
        perror("fcntl error\n");
        return;
    }

    if (fcntl(iFd, F_SETFL, iOpt | O_NONBLOCK) == -1)
    {
        perror("fcntl error\n");
        return;
    }
}
 int main(int argc, char **argv)
{

    string localIP(DEFAULT_LOCAP_IP), remoteIP(DEFAULT_REMOTE_IP);
    int port = DEFAULT_REMOTE_PORT, connectCount = DEFAULT_CONNECT_COUNT;
    int sleepCount = -1, sleepTime = -1, help = 0;
    int opt;
    while ((opt = getopt(argc, argv, "l:r:p:c:s:t:p:h")) != -1)
    {
        switch (opt)
        {
        case 'l':
            localIP = optarg;
            break;
        case 'r':
            remoteIP = optarg;
            break;
        case 'p':
            port = std::stoi(optarg);
            break;
        case 'c':
            connectCount = std::stoi(optarg);
            break;
        case 's':
            sleepCount = std::stoi(optarg);
            break;
        case 't':
            sleepTime = std::stoi(optarg);
            break;
        case 'h':
            help = 1;
            break;
        }
    }
    if (help)
    {
        printf("usage: client -l:localIP -r:remoteIp -p:remote port -c:connect count -s:stop count -t:stop time\n");
        return 0;
    }

    printf("paramaters localIP:%s remoteIP:%s remote port:%d connect count:%d stop count:%d stop time:%ds\n",
           localIP.c_str(), remoteIP.c_str(), port, connectCount, sleepCount, sleepTime);

    CEpoll g_epoll(OnRead, OnWrite, OnError);
    int sockfd;
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;
    int szfd[NUM_SOCKET];

    count = 0;

    int connectTimes = 0;
    for (int index = 0; index < connectCount; ++index)
    {
        if ((szfd[index] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("creat socket fail index:%d, fd:%d, error:%s\n", index, szfd[index], strerror(errno));
            //sleep(60);
        }
         else
        {
            printf("creat socket success index:%d, fd:%d\n", index, szfd[index]);
        } 
        if (strlen(localIP.c_str()) > 0)
        {
            struct sockaddr_in localaddr;
            localaddr.sin_family = AF_INET;
            localaddr.sin_addr.s_addr = inet_addr(localIP.c_str());
            localaddr.sin_port = 0; // Any local port will do
            bind(szfd[index], (struct sockaddr *)&localaddr, sizeof(localaddr));
        }

        int flag = 1;
        int ret = setsockopt(szfd[index], IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
         if (ret == -1)
        {
            printf("setsockopt fail index:%d, fd:%d, error:%s\n", index, szfd[index], strerror(errno));
            //sleep(60);
        } 

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(remoteIP.c_str());
        servaddr.sin_port = htons(port);
         if (inet_pton(AF_INET, remoteIP.c_str(), &servaddr.sin_addr) <= 0)
        {
            printf("inet_pton error for %s, error%s\n", remoteIP.c_str(), strerror(errno));
            //sleep(60);
        } 

        //g_epoll.AddFdOnWrite(szfd[index]);

        SetNoblock(szfd[index]);
        //connect(szfd[index], (const sockaddr *)&servaddr, sizeof(servaddr));

         if (connect(szfd[index], (const sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS)
            {
                printf("no new connect inedx:%d, error:%s\n", index, strerror(errno));
            }
            else
            {
                printf("connect fail index:%d, fd:%d, errno:%d, error:%s\n", index, szfd[index], errno, strerror(errno));
            }
        }
        else
        {
            ++count;
            printf("socket connect success count:%d, fd:%d\n", count, szfd[index]);
        }
        ++connectTimes;
        if (sleepTime > 0 && sleepCount && connectTimes > 0 && connectTimes % sleepCount == 0)
        {
            sleep(sleepTime);
            printf("socket connect connectCount:%d\n", connectTimes);
        } 
    }

    //while (g_epoll.ProcessMsg());
     while(1){
        sleep(10);
    } 
} 


/* int main(int argc, char **argv)
{

    string localIP(DEFAULT_LOCAP_IP), remoteIP(DEFAULT_REMOTE_IP);
    int port = DEFAULT_REMOTE_PORT, connectCount = DEFAULT_CONNECT_COUNT;
    int sleepCount = -1, sleepTime = -1, help = 0;
    int opt;
    while ((opt = getopt(argc, argv, "l:r:p:c:s:t:p:h")) != -1)
    {
        switch (opt)
        {
        case 'l':
            localIP = optarg;
            break;
        case 'r':
            remoteIP = optarg;
            break;
        case 'p':
            port = std::stoi(optarg);
            break;
        case 'c':
            connectCount = std::stoi(optarg);
            break;
        case 's':
            sleepCount = std::stoi(optarg);
            break;
        case 't':
            sleepTime = std::stoi(optarg);
            break;
        case 'h':
            help = 1;
            break;
        }
    }
    if (help)
    {
        printf("usage: client -l:localIP -r:remoteIp -p:remote port -c:connect count -s:stop count -t:stop time\n");
        return 0;
    }

    printf("paramaters localIP:%s remoteIP:%s remote port:%d connect count:%d stop count:%d stop time:%ds\n",
           localIP.c_str(), remoteIP.c_str(), port, connectCount, sleepCount, sleepTime);

    CEpoll g_epoll(OnRead, OnWrite, OnError);
    int sockfd;
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;
    int szfd[NUM_SOCKET];

    count = 0;

    int connectTimes = 0;
    for (int index = 0; index < connectCount; ++index)
    {
        if ((szfd[index] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("creat socket fail index:%d, fd:%d, error:%s\n", index, szfd[index], strerror(errno));
            //sleep(60);
        }

        if (strlen(localIP.c_str()) > 0)
        {
            struct sockaddr_in localaddr;
            localaddr.sin_family = AF_INET;
            localaddr.sin_addr.s_addr = inet_addr(localIP.c_str());
            localaddr.sin_port = 0; // Any local port will do
            bind(szfd[index], (struct sockaddr *)&localaddr, sizeof(localaddr));
        }

        int flag = 1;
        int ret = setsockopt(szfd[index], IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(remoteIP.c_str());
        servaddr.sin_port = htons(port);

        SetNoblock(szfd[index]);
        connect(szfd[index], (const sockaddr *)&servaddr, sizeof(servaddr));

        ++connectTimes;
        if (sleepTime > 0 && sleepCount && connectTimes > 0 && connectTimes % sleepCount == 0)
        {
            sleep(sleepTime);
            printf("socket connect connectCount:%d\n", connectTimes);
        } 
    }
     while(1){
        sleep(10);
    } 
}
 */