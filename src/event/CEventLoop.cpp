#include "../../include/event/CEventLoop.h"
#include "../../include/orderservice/orderserviceglobal.h"
#define FIRED_ACCEPT_EVENT_SIZE 1

CEventLoop::~CEventLoop()
{
    LOG_DEBUG("In CEventLoop::~CEventLoop");
    deleteEventLoop();
    LOG_DEBUG("Out CEventLoop::~CEventLoop");
}

CEventLoop::CEventLoop(int setSize) : m_event(setSize), m_setSize(setSize)
{
    LOG_DEBUG("In CEventLoop::CEventLoop setSize:%d", setSize);
    createEventLoop();
    LOG_DEBUG("Out CEventLoop::CEventLoop");
}

bool CEventLoop::createEventLoop()
{
    LOG_DEBUG("In CEventLoop::createEventLoop");
    bool result = true;
    std::string errorMsg("");
    do
    {
        m_allEvents = (FileEvent *)malloc(sizeof(FileEvent) * m_setSize);
        if (!m_allEvents)
        {
            errorMsg = "Mallocl m_allEvents fail!";
            result = false;
            break;
        }
        m_firedEvents = (FiredEvent *)malloc(sizeof(FiredEvent) * m_setSize);
        if (!m_firedEvents)
        {
            errorMsg = "Mallocl m_firedEvents fail!";
            result = false;
            break;
        }

    } while (false);

    if (result == false)
    {
        if (m_allEvents)
        {
            free(m_allEvents);
            m_allEvents = NULL;
        }
        if (m_firedEvents)
        {
            free(m_firedEvents);
            m_firedEvents = NULL;
        }

        LOG_WARN("CEventLoop::createEventLoop fail errorMsg:%s", errorMsg.c_str());
    }

    LOG_DEBUG("ut CEventLoop::createEventLoop result:%d", result);

    return result;
}

bool CEventLoop::deleteEventLoop()
{
    LOG_DEBUG("In CEventLoop::deleteEventLoop\n");
    if (m_allEvents)
    {
        free(m_allEvents);
        m_allEvents = NULL;
    }

    if (m_firedEvents)
    {
        free(m_firedEvents);
        m_firedEvents = NULL;
    }


    LOG_DEBUG("Out CEventLoop::deleteEventLoop\n");
    return true;
}


bool CEventLoop::deleteFileEvent(int fd, int mask)
{
    LOG_DEBUG("In CEventLoop::deleteFileEvent fd:%d, mask:%d", fd, mask);
    bool result = true;
    do
    {
        if (fd >= m_setSize)
        {
            LOG_WARN("CEventLoop::deleteFileEvent() fail for fd >= m_setSize fd:%d, m_setSize:%d", fd, m_setSize);
            result = false;
            break;
        }

        // 取出文件事件结构
        FileEvent *fe = &m_allEvents[fd];

        // 未设置监听的事件类型，直接返回
        if (fe->mask == AE_NONE)
        {
            LOG_WARN("CEventLoop::deleteFileEvent() fail for fe->mask == AE_NONEd");
            result = false;
            break;
        }

        // 计算新掩码
        fe->mask = fe->mask & (~mask);
        if (fd == m_maxFd && fe->mask == AE_NONE)
        {
            /* Update the max fd */
            int j;

            for (j = m_maxFd - 1; j >= 0; j--)
                if (m_allEvents[j].mask != AE_NONE)
                    break;
            m_maxFd = j;
        }
    } while (false);

    // 取消对给定 fd 的给定事件的监视
    if (result)
    {
        result = m_event.ApiDelEvent(fd, mask, m_allEvents);
    }
    LOG_DEBUG("Out CEventLoop::deleteFileEvent result:%d", result);
    return result;
}

bool CEventLoop::processEvents(int flags)
{
    LOG_DEBUG("Out CEventLoop processEvents flags:%d", flags);
    bool result = true;
    try
    {
        int numevents;
        struct timeval timeout = {1, 0}; // 1.0 seconds
        
        numevents = m_event.ApiPoll(NULL, m_firedEvents);
        //globalShareMutex.lock();
       // CServer::AcceptHandler(0,nullptr,0);
       //globalShareMutex.unLock();


        for (int j = 0; j < numevents; j++)
        {
/*             if(globalServer.IsListenning(m_firedEvents[j].fd)) {
                LOG_DEBUG("CEventLoop processEvents listenning:%d", m_firedEvents[j].fd);
                continue;
            } */
            FileEvent *fe = &m_allEvents[m_firedEvents[j].fd];

            int mask = m_firedEvents[j].mask;
            int fd = m_firedEvents[j].fd;
            int rfired = 0;

            if (fe->mask & mask & AE_READABLE)
            {
                rfired = 1;
                fe->rfileProc(fd, fe->clientData, mask);
            }

            if (fe->mask & mask & AE_WRITABLE)
            {
                if (!rfired || fe->wfileProc != fe->rfileProc)
                    fe->wfileProc(fd, fe->clientData, mask);
            }
        }
    }
    catch (exception &e)
    {
        LOG_WARN("CEventLoop::processEvents catch");
        LOG_WARN("CEventLoop::processEvents exception:%s", e.what());
        result = false;
    }

    LOG_DEBUG("Out CEventLoop::processEvents result:%d", result);
    return result;
}
