#ifndef IEVENT_H
#define IEVENT_H

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
#include <vector>


/*
 * 文件事件状态
 */
// 未设置
#define AE_NONE 0
// 可读
#define AE_READABLE 1
// 可写
#define AE_WRITABLE 2

/*
 * 时间处理器的执行 flags
 */
// 文件事件
#define AE_FILE_EVENTS 1
// 时间事件
#define AE_TIME_EVENTS 2
// 所有事件
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
// 不阻塞，也不进行等待
#define AE_DONT_WAIT 4

typedef void aeFileProc(int fd, void *clientData, int mask);
typedef int aeTimeProc(long long id, void *clientData);
typedef void aeEventFinalizerProc(void *clientData);

typedef struct FileEvent
{
  int mask;
  aeFileProc *rfileProc;
  aeFileProc *wfileProc;
  void *clientData;

} FileEvent;

/* A fired event
 *
 * 已就绪事件
 */
typedef struct FiredEvent
{

  // 已就绪文件描述符
  int fd;

  // 事件类型掩码，
  // 值可以是 AE_READABLE 或 AE_WRITABLE
  // 或者是两者的或
  int mask;

} FiredEvent;


/* Time event structure
 *
 * 时间事件结构
 */
typedef struct TimeEvent {

    // 时间事件的唯一标识符
    long long id; /* time event identifier. */

    // 事件的到达时间
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */

    // 事件处理函数
    aeTimeProc *timeProc;

    // 事件释放函数
    aeEventFinalizerProc *finalizerProc;

    // 多路复用库的私有数据
    void *clientData;

    // 指向下个时间事件结构，形成链表
    struct TimeEvent *next;

} TimeEvent;

class IEvent
{
  public:
    virtual bool ApiCreate() = 0;
    virtual bool ApiFree() = 0;
    virtual bool ApiResize(int size) = 0;
    virtual bool ApiAddEvent(int fd, int mask, FileEvent* pfileEvent) = 0;
    virtual bool ApiDelEvent(int fd, int delmask, FileEvent* pfileEvent) = 0;
    virtual int ApiPoll(struct timeval *tvp,FiredEvent *pFiredEvent) = 0;
};

#endif




