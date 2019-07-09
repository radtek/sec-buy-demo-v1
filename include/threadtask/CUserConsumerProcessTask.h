#ifndef CUSER_CONSUMER_PROCESS_TASK_H
#define CUSER_CONSUMER_PROCESS_TASK_H
#include <stdio.h>
#include "../thread/CThreadTask.h"
#include "../memdb/CMemDBConnectionPool.h"
#include "../dao/CDiskDBConnPool.h"
#include "../messagequeue/CMsgProducer.h"
#include "../messagequeue/CMsgProducerPool.h"
#include "../thread/CThreadPool.h"
#include <stdlib.h>
#include <unistd.h>
#include "../dao/CTableUser.h"
#include "../dao/CTUserMessageQueueTask.h"
#include "../dao/CTableUserProductRelation.h"
#include "../threadtask/CUserServiceTaskCheckTask.h"
#include "../dao/CTUserMessageQueueTask.h"
//umsg_task_id, message_offset, status,order_id, product_id, user_id, shop_id, amount, total_money

typedef struct UserProcessTask
{
    int iUmsgTaskId;
    int iMessageOffset;
    int iStatus;
    int iOrderId;
    int iProductId;
    int iUserId;
    int iShopId;
    int iAmount;
    int iTotalMoney;
} UserProcessTask;

/*执行任务的类：设置任务数据并执行*/
class CUserConsumerProcessTask : public CThreadTask
{

public:
    CUserConsumerProcessTask() = default;
    CUserConsumerProcessTask(string taskName, string id) : CThreadTask(taskName, id) {}
    bool Run(CMsgProducer *pProducer);
    virtual ~CUserConsumerProcessTask() {}
};

//void MSGProducer();
#endif
