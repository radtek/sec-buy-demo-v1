#include "../../include/orderservice/CTcpServerSocket.h"
#include "../../include/log/log.h"
CTcpServerSocket::CTcpServerSocket(string IP, int port) : CSocket(IP, port, AF_INET, SOCK_STREAM, 0, -1)
{
    LOG_DEBUG("In CTcpServerSocket::CTcpServerSocket IP:%s, port:%d", IP.c_str(), port);
    LOG_DEBUG("Out CTcpServerSocket::CTcpServerSocket IP:%s, port:%d", IP.c_str(), port);
}

CTcpServerSocket::~CTcpServerSocket()
{
    LOG_DEBUG("In CTcpServerSocket::~CTcpServerSocket");
    LOG_DEBUG("Out CTcpServerSocket::~CTcpServerSocket");
}