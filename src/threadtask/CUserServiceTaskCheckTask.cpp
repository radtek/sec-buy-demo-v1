#include "../../include/threadtask/CUserConsumerProcessTask.h"
#include "../../include/log/log.h"
bool CUserTaskCheckTask::Run(CMsgProducer *pProducer)
{
    LOG_DEBUG("In CUserTaskCheckTask::Run");
    pthread_mutex_lock(&CThreadPool::m_mapTaskStatusMutex);
    bool result = true;
    CDiskDBConnPool *connPool = CDiskDBConnPool::GetInstance();
    CDiskDB *conn = connPool->GetConnection();
    vector<UMQTaskRecord *> records;
    if (conn == nullptr)
    {
        LOG_WARN("Out CUserTaskCheckTask::Run conn is empty");
        return false;
    }
    
    try
    {
        conn->TransationStart();
        CTableUserMessageQueueTask tableUmsgQueueTask(conn);
        int status = 0;
        
        tableUmsgQueueTask.QueryByStatus(status, records);
        vector<UMQTaskRecord *>::iterator it = records.begin();

        for (; it != records.end(); ++it)
        {
            CUserConsumerProcessTask *task = new CUserConsumerProcessTask("CUserConsumerProcessTask",((*it)->umsgTaskId));
            UserProcessTask *data = (UserProcessTask *)malloc(sizeof(struct UserProcessTask));

            data->iUmsgTaskId = std::stoi(((*it)->umsgTaskId).c_str());
            data->iMessageOffset = std::stoi(((*it)->messageOffset).c_str());
            data->iStatus = 0;
            data->iOrderId = std::stoi(((*it)->orderId).c_str());
            data->iProductId = std::stoi(((*it)->productId).c_str());
            data->iUserId = std::stoi(((*it)->userId).c_str());
            data->iShopId = std::stoi(((*it)->shopId).c_str());
            data->iAmount = std::stoi(((*it)->amount).c_str());
            data->iTotalMoney = std::stof(((*it)->totalMoney).c_str());
            task->setData(data);
            consumerThreadPool.AddTask(task);

            TaskStatus *taskStatus = (TaskStatus *)malloc(sizeof(TaskStatus));
            taskStatus->status = 0;
            taskStatus->id = "test";
            CThreadPool::m_mapTaskStatus.insert(pair<string, TaskStatus *>(taskStatus->id, taskStatus));

            delete (*it);
        }
    }
    catch (exception &e)
    {
        LOG_WARN("CUserTaskCheckTask::Run catch");
        LOG_WARN("CUserTaskCheckTask::Run exception:%s", e.what());
        conn->TransationRollback();
        result = false;
    }
    records.clear();
    connPool->ReleaseConnection(conn);
    pthread_mutex_unlock(&CThreadPool::m_mapTaskStatusMutex);
    LOG_DEBUG("Out CUserTaskCheckTask::Run result:%d", result);
    return result;
}
