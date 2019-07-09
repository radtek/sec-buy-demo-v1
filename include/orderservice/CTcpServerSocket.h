#ifndef CTCP_SERVER_SOCKET_H
#define CTCP_SERVER_SOCKET_H
#include "../event/CSocket.h"

class CTcpServerSocket:public CSocket
{              
public:
  CTcpServerSocket(string IP, int port);
  ~CTcpServerSocket();
};
#endif