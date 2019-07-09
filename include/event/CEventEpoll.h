#ifndef CEVENT_EPOLL_H
#define CEVENT_EPOLL_H
#include "IEvent.h"
#include "../log/log.h"

typedef struct ApiState
{
    int epfd;
    int setSize;
    struct epoll_event *events;

} ApiState;

class CEvent
{
  public:
    ApiState *m_apiState;
    int m_setSize;

    CEvent(int size);
    ~CEvent();
    bool ApiCreate();
    bool ApiFree();
    bool ApiResize(int size);
    bool ApiAddEvent(int fd, int mask, FileEvent* pfileEvent);
    bool ApiDelEvent(int fd, int delmask, FileEvent* pfileEvent);
    int ApiPoll(struct timeval *tvp,FiredEvent *pFiredEvents);
};

inline bool CEvent::ApiAddEvent(int fd, int mask, FileEvent *pfileEvent)
{
    LOG_DEBUG("In CEvent ApiAddEvent fd:%d, mask:%d", fd, mask);

    ApiState *apiState = (ApiState *)m_apiState;
    struct epoll_event ee;
    int op = pfileEvent[fd].mask == AE_NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    ee.events = 0;
    mask |= pfileEvent[fd].mask;
    if (mask & AE_READABLE)
        ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE)
        ee.events |= EPOLLOUT;
    ee.data.u64 = 0;
    ee.data.fd = fd;
    bool result = true;
    if (epoll_ctl(apiState->epfd, op, fd, &ee) == -1){
         LOG_WARN("CEvent ApiAddEvent epoll_ctl false op:%d, fd:%d, error:%s", op, fd,strerror(errno));
         //printf("%s\n",strerror(errno));
        result = false;
    } 

    LOG_DEBUG("Out CEvent ApiAddEvent result:%d", result);
    return result;
}

#endif


