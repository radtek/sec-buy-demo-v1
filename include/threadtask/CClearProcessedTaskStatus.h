#ifndef CCLEAR_PROCESSED_TASK_STATUS_H
#define CCLEAR_PROCESSED_TASK_STATUS_H
#include <stdio.h>
#include "../thread/CThreadTask.h"
#include "../memdb/CMemDBConnectionPool.h"
#include "../dao/CDiskDBConnPool.h"
#include "../messagequeue/CMsgProducer.h"
#include "../messagequeue/CMsgProducerPool.h"
#include "../thread/CThreadPool.h"
#include <stdlib.h>
#include <unistd.h>
//umsg_task_id, message_offset, status,order_id, product_id, user_id, shop_id, amount, total_money


/*执行任务的类：设置任务数据并执行*/
class CClearProcessedTaskStatus : public CThreadTask
{

public:
    CClearProcessedTaskStatus() = default;
    CClearProcessedTaskStatus(string taskName, string id) : CThreadTask(taskName, id) {}
    bool Run(CMsgProducer *pProducer);
    virtual ~CClearProcessedTaskStatus() {}
};

//void MSGProducer();
#endif
