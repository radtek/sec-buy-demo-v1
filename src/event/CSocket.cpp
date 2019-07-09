#include "../../include/event/CSocket.h"

bool CSocket::Create()
{
    LOG_DEBUG("In CSocket Create");
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(m_port);
    m_addr.sin_addr.s_addr = inet_addr(m_IP.c_str());
    bzero(&(m_addr.sin_zero), 8);
    m_clientSinSize = sizeof(m_clientAddr);
   
    m_fd = socket(m_domain, m_type, m_protocol);
    LOG_DEBUG("CSocket::Create() m_fd:%d", m_fd);
    bool result = m_fd < 0 ? false : true;
    LOG_DEBUG("Out CSocket Create result:%d", result);
    return result;
}

bool CSocket::Close()
{
    LOG_DEBUG("In CSocket Close fd:%d", m_fd);
    close(m_fd);
    LOG_DEBUG("Out CSocket Close");
    return true;
}

bool CSocket::SetReuseAddr()
{
    LOG_DEBUG("In CSocket SetReuseAddr fd:%d", m_fd);
    int yes = 1;
    bool result = true;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
        LOG_WARN("CSocket::SetReuseAddr setsockopt fail!");
        result = false;
    }
    LOG_DEBUG("Out CSocket SetReuseAddr result:%d", result);
    return result;
}

bool CSocket::Bind()
{
    LOG_DEBUG("In CSocket Bind fd:%d", m_fd);
    bool result = true;
    if (bind(m_fd, (struct sockaddr *)&m_addr, sizeof(struct sockaddr)) == -1)
    {
        LOG_WARN("CSocket::Bind bind fail fd:%d!", m_fd);
        close(m_fd);
        result = false;
    }
    LOG_DEBUG("Out CSocket Bind result:%d", result);
    return result;
}

bool CSocket::Listen()
{
    LOG_DEBUG("In CSocket Listen fd:%d", m_fd);
    bool result = true;

    if (listen(m_fd, 65535) == -1)
    {
        LOG_WARN("Out CSocket Listen listen fasle fd:%d backlog:511", m_fd);
        close(m_fd);
        result = false;
    }
    LOG_DEBUG("Out CSocket Listen result:%d", result);
    return result;
}

int CSocket::Recv(char *buf, int length)
{
    LOG_DEBUG("In CSocket Listen fd:%d length:%d", m_fd, length);

    int recvNum = recv(m_fd, buf, length, 0);
    LOG_DEBUG("Out CSocket Listen fd:%d length:%d, recvNum:%d", m_fd, length, recvNum);
    return recvNum;
}

bool CSocket::SetNonBlock()
{
    LOG_DEBUG("In CSocket SetNonBlock fd:%d", m_fd);
    int flags;
    bool result = true;

    do
    {
        if ((flags = fcntl(m_fd, F_GETFL)) == -1)
        {
            LOG_WARN("CSocket::SetNonBlock fcntl F_GETFL fd:%d fail", m_fd);
            result = false;
            break;
        }
        if (fcntl(m_fd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            LOG_WARN("CSocket::SetNonBlock fcntl F_SETFL fd:%d fail", m_fd);
            result = false;
            break;
        }
    } while (false);
    LOG_DEBUG("Out CSocket SetNonBloc fd:%d result:%d", m_fd, result);
    return result;
}

int CSocket::AcceptTryBest()
{
    LOG_DEBUG("In CSocket Accept server fd:%d", m_fd);
    int fd;
    int sin_size = sizeof(m_clientAddr);
    memset(&m_clientAddr, 0, sizeof(struct sockaddr_in));
    //fd = accept(m_fd, (struct sockaddr *)&m_clientAddr, (socklen_t *)&sin_size);
     while (1)
    {
        fd = accept(m_fd, (struct sockaddr *)&m_clientAddr, (socklen_t *)&sin_size);
        if (fd == -1)
        {
            if (errno == EINTR){
                LOG_WARN("CSocket::Accept accept errno == EINTR");
                continue;
            }
            else
            {
                LOG_WARN("CSocket::Accept accept fail fd:%d, error:%s", m_fd,strerror(errno));
                //sleep(60);
                //LOG_WARN("CSocket::Accept accept fail");
                return -1;
            }
        }
        break;
    } 
    LOG_DEBUG("CSocket::Accept accepted fd:%d", fd);
    LOG_DEBUG("Out CSocket Accept server");
    return fd;
}

CSocket::~CSocket()
{
    LOG_DEBUG("In CSocket ~CSocket fd:%d", m_fd);
    Close();
    LOG_DEBUG("Out CSocket ~CSocket fd:%d", m_fd);
}

CSocket::CSocket(string IP, int port, int domain, int type, int protocol, int fd) : m_IP(IP), m_port(port), m_domain(domain),
                                                                                    m_type(type), m_protocol(protocol), m_fd(fd)
{
    gettimeofday(&m_lastCommTime, NULL);
/*     LOG_DEBUG("In CSocket CSocket IP:%s, port:%d, domain:%d, type:%d, protocol:%d, fd:%d", IP.c_str(), port, domain, type, protocol, fd);
    LOG_DEBUG("Out CSocket CSocket IP:%s, port:%d, domain:%d, type:%d, protocol:%d, fd:%d", IP.c_str(), port, domain, type, protocol, fd); */
}
