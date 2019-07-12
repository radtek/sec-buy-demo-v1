#ifndef CSERVER_H
#define CSERVER_H

#include <string.h>
#include <string>
#include <sys/time.h>
#include "../event/CSocket.h"
#include "../event/CEventLoop.h"
#include "./CTcpAcceptedClientSocket.h"
#include "./CTcpServerSocket.h"
#include "../memdb/CRedis.h"
#include "../messagequeue/CKafka.h"
#include "../../include/log/log.h"
#include <map>
#include <list>
using namespace std;

#define MAXLINE 100

class CServer
{
private:
  CTcpServerSocket m_socket;
  list<int> m_waitClearClientList;
  list<int>::iterator m_waitClearClientIter;
  struct timeval m_nextClearClientTime;
  int m_clientCount;

public:
  CServer(string IP, int port, int eventSize);
  ~CServer();
  bool Init();
  bool SetAcceptTcpHandler();
  bool AeMain();
  bool AddClient(int fd);
  bool RemoveClient(int fd);
  bool IsListenning(int fd);

  CTcpAcceptedClientSocket &GetClient(int fd);

  static void AcceptHandler(int fd, void *privdata, int mask);
  static void AcceptHandlerTryBest(int fd, void *privdata, int mask);
  static void AcceptHandlerMutilpThread(int fd, void *privdata, int mask);
  bool processTimeEvents();

  CTcpAcceptedClientSocket *m_szClients;
  CEventLoop m_eventLoop;
};

inline bool CServer::AddClient(int fd)
{

  if (fd > -1)
  {

    m_szClients[fd].SetFd(fd);
    m_waitClearClientList.push_back(fd);
    ++m_clientCount;
    LOG_DEBUG("CServer AddClient client m_clientCount:%d, fd:%d", m_clientCount, fd);
    return true;
  }
  LOG_DEBUG("CServer AddClient client m_clientCount:%d, fd:%d", m_clientCount, fd);
  return false;
}

inline bool CServer::RemoveClient(int fd)
{
  if (fd > -1 && m_eventLoop.deleteFileEvent(fd, AE_READABLE|AE_WRITABLE) > 0)
  {
    m_szClients[fd].SetFd(-1);
    --m_clientCount;
    close(fd);
    LOG_DEBUG("CServer RemoveClient client m_clientCount:%d, fd:%d", m_clientCount, fd);
    return true;
  }
  LOG_DEBUG("Out CServer  RemoveClient client m_clientCount:%d, fd:%d", m_clientCount, fd);
  return false;
}
#endif