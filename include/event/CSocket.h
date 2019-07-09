#ifndef CSOCKET_H
#define CSOCKET_H
#include <string.h>
#include <string>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include "../dao/CDiskDBConnPool.h"
#include "../memdb/CMemDBConnectionPool.h"
#include "../thread/CThreadPool.h"
#include "../threadtask/CPlaceOrderTask.h"
#include "../log/log.h"


using namespace std;

class CSocket
{
protected:
  string m_IP;
  int m_port;
  int m_domain;
  int m_type;
  int m_protocol;
  int m_clientSinSize;
  struct sockaddr_in m_addr;
  struct sockaddr_in m_clientAddr;
  

public:
  int m_fd;
  CSocket(string IP, int port, int domain, int type, int protocol, int fd);
  ~CSocket();
  bool Create();
  bool SetReuseAddr();
  bool Listen();
  bool Close();
  bool SetFd(int fd);
  int Accept();
  int AcceptTryBest();
  bool Send();
  int Recv(char* buf, int length);
  bool Bind();
  bool SetNonBlock();
  struct timeval m_lastCommTime;
};

inline int CSocket::Accept()
{
    int acceptedFd = accept(m_fd, (struct sockaddr *)&m_clientAddr, (socklen_t *)&m_clientSinSize);
    gettimeofday(&m_lastCommTime, NULL);
    LOG_DEBUG("CSocket::Accept accepted m_fd:%d, acceptedFd:%d", m_fd, acceptedFd);
    return acceptedFd;
}

inline bool CSocket::SetFd(int fd){
    m_fd = fd;
    LOG_DEBUG("Out CSocket SetFd fd:%d", fd);
    return true;
}

#endif

