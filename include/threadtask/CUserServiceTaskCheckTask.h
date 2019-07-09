#ifndef CUSER_TASK_CHECK_TASK_H
#define CUSER_TASK_CHECK_TASK_H
#include <stdio.h>
#include "../thread/CThreadTask.h"
#include "../memdb/CMemDBConnectionPool.h"
#include "../dao/CDiskDBConnPool.h"
#include "../messagequeue/CMsgProducer.h"
#include "../messagequeue/CMsgProducerPool.h"
#include "../thread/CThreadPool.h"
#include "../userservice/userserviceglobal.h"
#include <stdlib.h>
#include <unistd.h>
//umsg_task_id, message_offset, status,order_id, product_id, user_id, shop_id, amount, total_money


/*执行任务的类：设置任务数据并执行*/
class CUserTaskCheckTask : public CThreadTask
{

public:
    CUserTaskCheckTask() = default;
    CUserTaskCheckTask(string taskName, string id) : CThreadTask(taskName, id) {}
     bool Run(CMsgProducer *pProducer);
    virtual ~CUserTaskCheckTask() {}
};

//void MSGProducer();
#endif
