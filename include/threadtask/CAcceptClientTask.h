#ifndef CACCEPT_CLIENT_TASK_H
#define CACCEPT_CLIENT_TASK_H
#include <stdio.h>
#include "../thread/CThreadTask.h"
#include "../messagequeue/CMsgProducer.h"
#include "../messagequeue/CMsgProducerPool.h"
#include <stdlib.h>
#include <unistd.h>


typedef struct AcceptClintTaskData {
    int len;
    int szFd[0];
} AcceptClintTaskData;


/*执行任务的类：设置任务数据并执行*/
class CAcceptClientTask : public CThreadTask
{
private:
    int m_ClientFd;

public:
    CAcceptClientTask() = default;
    CAcceptClientTask(string taskName, string id) : CThreadTask(taskName, id) {}
    bool Run(CMsgProducer *pProducer);
    virtual ~CAcceptClientTask();
};

//void MSGProducer();
#endif
