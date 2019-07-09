#include "../../include/threadtask/CUserConsumerProcessTask.h"
#include "../../include/log/log.h"
#include "../../include/idservice/idhelper.h"
bool CUserConsumerProcessTask::Run(CMsgProducer *pProducer)
{
    LOG_DEBUG("In CUserConsumerProcessTask::Run");
    bool result = true;
    CDiskDBConnPool *connPool = CDiskDBConnPool::GetInstance();
    CDiskDB *conn = connPool->GetConnection();
    if (conn == nullptr)
    {
        LOG_WARN("Out CUserConsumerProcessTask::Run conn is empty");
        return false;
    }
    std::string errorMsg("");
    int umsgTaskId = 0;
    try
    {
        conn->TransationStart();
        UserProcessTask *task = (UserProcessTask *)m_ptrData;
        CTableUser tableUser(conn);
        CTableUserProductRelation tableRelation(conn);
        CTableUserMessageQueueTask tableTask(conn);
        umsgTaskId = task->iUmsgTaskId;
        do
        {
            LOG_DEBUG("CUserConsumerProcessTask::Run process business begin id:%ld", task->iUmsgTaskId);
            UserRecord userRecord;
            if (!tableUser.QueryByUserID(task->iUserId, userRecord))
            {
                errorMsg = "table user query fail!";
                result = false;
                break;
            }
            float userMoney = std::stof(userRecord.money);

            UserProductRelationRecord relationRecord;
            if (!tableRelation.QueryByProductIdAndUserId(task->iUserId, task->iProductId, relationRecord))
            {
                errorMsg = "table relation query fail!";
                //result = false;
                //break;
            }
            else
            {
                enum idTypeEnum idType = user_product_relation_id;
                unsigned long relationId = getId(idType);
                relationRecord.relationId = std::to_string(relationId);
                relationRecord.productId = std::to_string(task->iProductId);
                relationRecord.userId = std::to_string(task->iUserId);
                relationRecord.amount = "0";
                tableRelation.Insert(relationRecord);
            }

            int amount = std::stoi(relationRecord.amount);
            unsigned long relationId = std::stol(relationRecord.relationId);

            float newMoney = userMoney - task->iTotalMoney;
            int newAmount = amount + task->iAmount;

            if (!tableUser.UpdateMoneyByUserID(newMoney, task->iUserId))
            {
                errorMsg = "table user update fail!";
                result = false;
                break;
            }
            if (!tableRelation.UpdateAmountByRelationId(newAmount, relationId))
            {
                errorMsg = "table relation update fail!";
                result = false;
                break;
            }
            int status = 1;
            if (!tableTask.UpdateTaskStatus(status, task->iOrderId, task->iUserId, task->iProductId))
            {
                errorMsg = "table task update fail!";
                result = false;
                break;
            }
            LOG_DEBUG("CUserConsumerProcessTask::Run process business end");

        } while (0);
    }
    catch (exception &e)
    {
        LOG_WARN("CUserTaskCheckTask::Run process business catch");
        LOG_WARN("CUserTaskCheckTask::Run process business exception:%s", e.what());
        errorMsg = e.what();
        conn->TransationRollback();
        result = false;
    }

    if (result)
    {
        LOG_DEBUG("CUserConsumerProcessTask::Run process task status map begin");
        pthread_mutex_lock(&CThreadPool::m_mapTaskStatusMutex);
        try
        {
            map<string, TaskStatus *>::iterator it;
            it = CThreadPool::m_mapTaskStatus.find(m_id);
            if (it == CThreadPool::m_mapTaskStatus.end())
            {
                LOG_DEBUG("CUserConsumerProcessTask::Run can not find m_id:%s", m_id.c_str());
            }
            else
            {
                LOG_DEBUG("CUserConsumerProcessTask::Run found m_id:%s", m_id.c_str());
                (it->second)->status = 1;
            }
            conn->TransationCommit();
            LOG_DEBUG("CUserConsumerProcessTask::Run process task status map end");
        }
        catch (exception &e)
        {
            LOG_WARN("CUserTaskCheckTask::Run process task status map catch");
            LOG_WARN("CUserTaskCheckTask::Run process task status map exception:%s", e.what());
            conn->TransationRollback();
            result = false;
        }
        pthread_mutex_unlock(&CThreadPool::m_mapTaskStatusMutex);
    }
    else
    {
        LOG_WARN("CUserConsumerProcessTask::Run errorMsg:%s", errorMsg.c_str());
        LOG_DEBUG("CUserConsumerProcessTask::Run will not process task status map");
        conn->TransationRollback();
    }

    connPool->ReleaseConnection(conn);
    LOG_DEBUG("Out CUserConsumerProcessTask::Run  id:%ld, result:%d", umsgTaskId, result);
    return result;
}
