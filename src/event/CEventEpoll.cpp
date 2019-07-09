#include "../../include/event/CEventEpoll.h"
#include "../../include/log/log.h"

CEvent::CEvent(int size) : m_setSize(size)
{
    LOG_DEBUG("In CEvent CEvent size:%d", size);
    ApiCreate();
    LOG_DEBUG("Put CEvent CEvent size:%d", size);
}

CEvent::~CEvent()
{
    LOG_DEBUG("In CEvent ~CEvent");
    ApiFree();
    LOG_DEBUG("Out CEvent ~CEvent");
}

bool CEvent::ApiCreate()
{
    LOG_DEBUG("In CEvent ApiCreate");
    bool result = true;
    std::string errorMsg("");
    ApiState *apiState = NULL;
    do
    {
        
        apiState = (ApiState *)malloc(sizeof(ApiState));
        if (!apiState)
        {
            errorMsg = "Malloc ApiState fail!";
            result = false;
            break;
        }
        apiState->events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * m_setSize);
        if (!apiState->events)
        {
            errorMsg = "Malloc epoll_event fail!";
            result = false;
            break;
        }
        apiState->epfd = epoll_create(1024);
        if (apiState->epfd == -1)
        {
            errorMsg = "epoll_create fail!";
            result = false;
            break;
        }
        apiState->setSize = m_setSize;
        m_apiState = apiState;
    } while (false);

    

    if (result == false)
    {
        if (apiState->events)
        {
            free(apiState->events);
            apiState->events = NULL;
        }
        if (apiState)
        {
            free(apiState);
            apiState = NULL;
        }
        LOG_WARN("CEvent ApiCreate fail error:%s", errorMsg.c_str());
    }

    LOG_DEBUG("Out CEvent ApiCreate");
    return result;
}

bool CEvent::ApiFree()
{
    LOG_DEBUG("In CEvent ApiFree");
    ApiState *apiState = (ApiState *)m_apiState;
    if (apiState)
    {
        if (apiState->epfd > 0)
        {
            close(apiState->epfd);
        }

        if (apiState->events)
        {
            free(apiState->events);
            apiState->events = NULL;
        }

        free(apiState);
        apiState = NULL;
    }
    LOG_DEBUG("Out CEvent ApiFree");
    return true;
}

bool CEvent::ApiResize(int setsize)
{
    LOG_DEBUG("In CEvent ApiResize setsize:%d", setsize);
    ApiState *apiState = (ApiState *)m_apiState;
    apiState->setSize = setsize;
    m_setSize = setsize;
    apiState->events = (struct epoll_event *)realloc(apiState->events, sizeof(struct epoll_event) * setsize);
    if(apiState->events == nullptr) {
        LOG_WARN("CEvent ApiResize realloc fail!");
    } else {
        LOG_DEBUG("CEvent ApiResize realloc success!");
    }
    LOG_DEBUG("Out CEvent ApiResize");
    return true;
}


bool CEvent::ApiDelEvent(int fd, int delmask, FileEvent *pfileEvent)
{
    LOG_DEBUG("In CEvent ApiDelEvent fd:%d, delmask:%d", fd, delmask);
    ApiState *pState = (ApiState *)m_apiState;
    struct epoll_event ee;
    int mask = pfileEvent[fd].mask & (~delmask);

    ee.events = 0;
    if (mask & AE_READABLE)
        ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE)
        ee.events |= EPOLLOUT;
    ee.data.u64 = 0; /* avoid valgrind warning */
    ee.data.fd = fd;
    bool result = true;
    if (mask != AE_NONE)
    {
        if (epoll_ctl(pState->epfd, EPOLL_CTL_MOD, fd, &ee) == -1)
        {
            LOG_WARN("CEvent ApiDelEvent epoll_ctl EPOLL_CTL_MOD false", fd, mask);
            result = false;
        }
    }
    else
    {
        if (epoll_ctl(pState->epfd, EPOLL_CTL_DEL, fd, &ee) == -1)
        {
            LOG_WARN("CEvent ApiDelEvent epoll_ctl EPOLL_CTL_DEL false", fd, mask);
            result = false;
        }
    }
    LOG_DEBUG("Out CEvent ApiDelEvent result:%d", result);
    return result;
}

int CEvent::ApiPoll(struct timeval *tvp, FiredEvent *pFiredEvent)
{
    LOG_DEBUG("In CEvent ApiPoll");
    ApiState *pState = (ApiState *)m_apiState;
    int retval, numevents = 0;

    // 等待时间
    retval = epoll_wait(pState->epfd, pState->events, m_setSize,
                        tvp ? (tvp->tv_sec * 1000 + tvp->tv_usec / 1000) : -1);

    LOG_DEBUG("CEvent ApiPoll epoll_wait:%d", retval);
    // sleep(5);
    // 有至少一个事件就绪？
    if (retval > 0)
    {
        int index;

        // 为已就绪事件设置相应的模式
        // 并加入到 eventLoop 的 fired 数组中
        numevents = retval;
        for (index = 0; index < numevents; index++)
        {
            int mask = 0;
            struct epoll_event *e = pState->events + index;

            if (e->events & EPOLLIN)
                mask |= AE_READABLE;
            if (e->events & EPOLLOUT)
                mask |= AE_WRITABLE;
            if (e->events & EPOLLERR)
                mask |= AE_WRITABLE;
            if (e->events & EPOLLHUP)
                mask |= AE_WRITABLE;

            pFiredEvent[index].fd = e->data.fd;
            pFiredEvent[index].mask = mask;
            LOG_DEBUG("CEvent ApiPoll index:%d, fd:%d, mak:%d", index, e->data.fd, mask);
        }
    }
    LOG_DEBUG("Out CEventLoop ApiPoll");
    // 返回已就绪事件个数
    return numevents;
}
