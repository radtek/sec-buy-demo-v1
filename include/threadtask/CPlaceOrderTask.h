#ifndef CPLACE_ORDER_TASK_H
#define CPLACE_ORDER_TASK_H
#include <stdio.h>
#include "../thread/CThreadTask.h"
#include "../memdb/CMemDBConnectionPool.h"
#include "../dao/CDiskDBConnPool.h"
#include "../messagequeue/CMsgProducer.h"
#include "../messagequeue/CMsgProducerPool.h"
#include "../dao/CTableBuyOrder.h"
#include <stdlib.h>
#include <unistd.h>

typedef struct Order
{
    int clientFd;
    int userID;
    int shopID;
    int productID;
    int number;
} Order;

/*执行任务的类：设置任务数据并执行*/
class CPlaceOrderTask : public CThreadTask
{
private:
    int m_ClientFd;

public:
    CPlaceOrderTask() = default;
    CPlaceOrderTask(string taskName, string id) : CThreadTask(taskName, id) {}
    bool Run(CMsgProducer *pProducer);
    virtual ~CPlaceOrderTask() {}
};

//void MSGProducer();
#endif
