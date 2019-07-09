#ifndef CEVENT_LOOP_H
#define CEVENT_LOOP_H
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "CEventEpoll.h"
#include "../log/log.h"
using namespace std;


class CEventLoop
{
private:
  int m_maxFd;
  int m_stop;
  int m_setSize;
  FileEvent *m_allEvents;
  FiredEvent *m_firedEvents;
  CEvent m_event;
  void *m_apidata;


public:
  CEventLoop(int setSize);
  ~CEventLoop();
  bool createEventLoop();
  bool deleteEventLoop();
  bool createFileEvent(int fd, int mask,
                      aeFileProc *proc, void *clientData);
  bool deleteFileEvent(int fd, int mask);
  bool processEvents(int flags);
  bool processTimeEvents();
};

inline bool CEventLoop::createFileEvent(int fd, int mask,
                                 aeFileProc *proc, void *clientData)
{
    LOG_DEBUG("In CEventLoop::createFileEvent fd:%d, mask:%d", fd, mask);
    bool result = true;
    do
    {
        if (fd >= m_setSize)
        {
            LOG_WARN("CEventLoop::createFileEvent fail for fd >= m_setSize fd:%d, m_setSize:%d", fd, m_setSize);
            errno = ERANGE;
            result = false;
            break;
        }

        // 取出文件事件结构
        FileEvent *fe = &m_allEvents[fd];

        // 监听指定 fd 的指定事件
        if (!m_event.ApiAddEvent(fd, mask, m_allEvents))
        {
            LOG_WARN("CEventLoop::createFileEvent m_event.ApiAddEvent fail");
            result = false;
            break;
        }

        // 设置文件事件类型，以及事件的处理器
        fe->mask |= mask;
        if (mask & AE_READABLE)
            fe->rfileProc = proc;
        if (mask & AE_WRITABLE)
            fe->wfileProc = proc;

        // 私有数据
        fe->clientData = clientData;

        // 如果有需要，更新事件处理器的最大 fd
        if (fd > m_maxFd)
            m_maxFd = fd;
    } while (false);

    LOG_DEBUG("Out CEventLoop::createFileEvent %d", result);
    return result;
}
#endif
