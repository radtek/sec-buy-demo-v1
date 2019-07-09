#ifndef CTCP_ACCEPTED_CLIENT_SOCKET_H
#define CTCP_ACCEPTED_CLIENT_SOCKET_H
#include "../event/CSocket.h"
class CTcpAcceptedClientSocket:public CSocket
{              
public:
  CTcpAcceptedClientSocket(int fd);
  CTcpAcceptedClientSocket();
  ~CTcpAcceptedClientSocket();
  static void RecvHandler(int fd, void *privdata, int mask);
};
#endif