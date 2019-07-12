#include "../../include/orderservice/CServer.h"
#include "../../include/orderservice/orderserviceglobal.h"
#include "../../include/threadtask/CAcceptClientTask.h"

#define ACCEPT_TASK_MAX_SIZE 1000
int *g_szFd = (int *)malloc(sizeof(int) * ACCEPT_TASK_MAX_SIZE);

CServer::CServer(string IP, int port, int eventSize) : m_clientCount(0), m_socket(IP, port), m_eventLoop(eventSize)
{
    LOG_DEBUG("In CServer  CServer IP:%s, port:%d, eventSize:%d", IP.c_str(), port, eventSize);
    m_szClients = new CTcpAcceptedClientSocket[eventSize];
    m_waitClearClientIter = m_waitClearClientList.begin();
    gettimeofday(&m_nextClearClientTime, NULL);
    m_nextClearClientTime.tv_sec = m_nextClearClientTime.tv_sec + globalConfig.m_clearClientInterval;
    LOG_DEBUG("Out CServer  CServer IP:%s, port:%d, eventSize:%d", IP.c_str(), port, eventSize);
}

CServer::~CServer()
{

    LOG_DEBUG("In CServer  ~CServer()");

    if (m_szClients)
    {
        delete[] m_szClients;
        m_szClients = nullptr;
    }
    LOG_DEBUG("Out CServer  ~CServer()");
}

bool CServer::Init()
{
    LOG_DEBUG("In CServer  Init()");
    bool result = true;
    std::string errorMsg("");
    do
    {
        if (m_socket.Create() == false)
        {
            errorMsg = "Socket create fail!";
            result = false;
            break;
        };
        if (m_socket.SetReuseAddr() == false)
        {
            errorMsg = "Socket set rreuse address fail!";
            result = false;
            break;
        };
        if (m_socket.SetNonBlock() == false)
        {
            errorMsg = "Socket set non block fail!";
            result = false;
            break;
        };
        if (m_socket.Bind() == false)
        {
            errorMsg = "Socket bind fail!";
            result = false;
            break;
        };
        if (m_socket.Listen() == false)
        {
            errorMsg = "Socket listen fail!";
            result = false;
            break;
        };
    } while (false);

    if (result == false)
    {
        LOG_WARN("CServer Init() fail error message:%s", errorMsg.c_str());
        //sleep(3000);
        exit(0);
    }
    else
    {
        LOG_DEBUG("CServer Init() success!");
    }
    LOG_DEBUG("Out CServer  Init()");
    return result;
}

bool CServer::IsListenning(int fd)
{
    LOG_DEBUG("In CServer  IsListenning fd:%d", fd);
    bool result = false;
    if (m_socket.m_fd == fd)
    {
        result = true;
    }
    LOG_DEBUG("Out CServer IsListenning fd:%d, result:%d", fd, result);
    return result;
}

void CServer::AcceptHandlerMutilpThread(int fd, void *privdata, int mask)
{
    LOG_DEBUG("In CServer  AcceptHandlerMutilpThread fd:%d, mask:%d", fd, mask);
    CEventLoop &eventLoop = globalServer.m_eventLoop;
    CSocket &socket = globalServer.m_socket;
    int count = 0;
    while (count < globalConfig.m_acceptClientBatchNumber)
    {
        int cliFd = socket.Accept();
        LOG_DEBUG("CServer AcceptHandlerMutilpThread cliFd:%d", cliFd);
        if (cliFd > 0)
        {
            g_szFd[count] = cliFd;
            ++count;
        }
        else
        {
            break;
        }
    }
    if (count > 0)
    {
        int size = sizeof(int) * (1 + count);
        AcceptClintTaskData *data = (AcceptClintTaskData *)malloc(size);
        if (data)
        {
            data->len = count;
            memcpy(data->szFd, g_szFd, size - sizeof(int));
            CAcceptClientTask *task = new CAcceptClientTask("CAcceptClientTask", "");
            task->setData(data);
            orderThreadPool.AddTask(task);
        }
        else
        {
            LOG_ERROR("CServer AcceptHandlerMutilpThread malloc AcceptClintTaskData fail!");
        }
    }

    LOG_DEBUG("Out CServer AcceptHandlerMutilpThread accept count:%d", count);
}

void CServer::AcceptHandler(int fd, void *privdata, int mask)
{
    LOG_DEBUG("In CServer  AcceptHandler fd:%d, mask:%d", fd, mask);
    int count = 0;
    CEventLoop &eventLoop = globalServer.m_eventLoop;
    CSocket &socket = globalServer.m_socket;
    while (count < globalConfig.m_acceptClientBatchNumber)
    {
        int cliFd = socket.Accept();
        if (cliFd > 0)
        {
            if (eventLoop.createFileEvent(cliFd, AE_READABLE, CTcpAcceptedClientSocket::RecvHandler, nullptr)  > 0)
            {
                globalServer.AddClient(cliFd);
                ++count;
                LOG_DEBUG("CServer AcceptHandler createFileEvent success index:%d!", count);
            }
            else
            {
                printf("gary_test eventLoop.createFileEvent\n");
                sleep(1000);
                LOG_WARN("CServer AcceptHandler close cliFd:%d", cliFd);
                close(cliFd);
            }
        }
        else
        {
            LOG_WARN("CServer AcceptHandler accept fail errno:%d, error:%s", errno, strerror(errno));
            break;
        }
    }

    LOG_WARN("Out CServer AcceptHandler accept count:%d", count);
}

bool CServer::SetAcceptTcpHandler()
{
    LOG_DEBUG("In CServer  SetAcceptTcpHandler");
    bool ret = m_eventLoop.createFileEvent(m_socket.m_fd, AE_READABLE, CServer::AcceptHandler, nullptr);
    if (ret == true)
    {
        LOG_DEBUG("CServer::SetAcceptTcpHandler() createFileEvent for fd:%d AcceptHandler success\n", m_socket.m_fd);
    }
    else
    {
        LOG_WARN("CServer::SetAcceptTcpHandler() createFileEvent for fd:%d AcceptHandler fail\n", m_socket.m_fd);
    }
    LOG_DEBUG("Out CServer  SetAcceptTcpHandler()");
    return ret;
}

bool CServer::AeMain()
{
    LOG_DEBUG("In CServer  AeMain()");
    while (true)
    {
        // 开始处理事件
        m_eventLoop.processEvents(AE_ALL_EVENTS);
        processTimeEvents();
    }
    LOG_DEBUG("Out CServer  AeMain()");
    return true;
}

CTcpAcceptedClientSocket &CServer::GetClient(int fd)
{
    if (fd > -1)
    {
        return m_szClients[fd];
    }
    else
    {
        return m_szClients[0];
    }
}

bool CServer::processTimeEvents()
{
    if (m_waitClearClientList.size() == 0)
    {
        return false;
    }
    struct timeval tv;
    gettimeofday(&tv, NULL);
    LOG_DEBUG("processTimeEvents process m_waitClearClientList.size:%d now time:%d,  next time:%d", m_waitClearClientList.size(), tv.tv_sec, m_nextClearClientTime.tv_sec);
    if (tv.tv_sec < m_nextClearClientTime.tv_sec)
    {
        LOG_DEBUG("processTimeEvents process not reach time");
        return false;
    }
    LOG_DEBUG("processTimeEvents process reach time");

    int count = 0;
    list<int>::iterator iterLastTime = m_waitClearClientIter;
    list<int>::iterator end = m_waitClearClientList.end();
    while (count < globalConfig.m_clearClientBatchNumber)
    {
        //do something

        if (m_waitClearClientIter == end)
        {
            m_waitClearClientIter = m_waitClearClientList.begin();
        }
        else
        {
            ++count;
            int fd = *m_waitClearClientIter;
            if (m_szClients[fd].m_fd == -1)
            {
                m_waitClearClientIter = m_waitClearClientList.erase(m_waitClearClientIter);
            }
            else if (m_szClients[fd].m_fd > -1 && tv.tv_sec >= m_szClients[fd].m_lastCommTime.tv_sec)
            {
                globalServer.RemoveClient(fd);
                m_waitClearClientIter = m_waitClearClientList.erase(m_waitClearClientIter);
            }
            else
            {
                ++m_waitClearClientIter;
            }
        }

        if (m_waitClearClientIter == iterLastTime)
        {
            LOG_DEBUG("processTimeEvents break for duplicate in count:%d", count);
            break;
        }
    }
    m_nextClearClientTime.tv_sec = m_nextClearClientTime.tv_sec + globalConfig.m_clearClientInterval;
    LOG_DEBUG("processTimeEvents process count:%d", count);
}
