#include "../../include/dao/CTUserMessageQueueTask.h"
#include "../../include/log/log.h"

CTableUserMessageQueueTask::CTableUserMessageQueueTask(CDiskDB *conn) : m_conn(conn)
{
    if (conn)
    {
        LOG_DEBUG("In CTableUserMessageQueueTask::CTableUserMessageQueueTask conn id:%d", conn->m_id);
    }
    else
    {
        LOG_WARN("In CTableUserMessageQueueTask::CTableUserMessageQueueTask conn is empty");
    }
}

bool CTableUserMessageQueueTask::UpdateTaskStatus(const int status, const int orderId, const int userId, const int productId)
{
    LOG_DEBUG("In CTableUserMessageQueueTask::UpdateTaskStatus status:%d, orderId:%d, userId:%d, productId:%d", status, orderId, userId, productId);
    char szUpdateTaskStatusSql[300];
    sprintf(szUpdateTaskStatusSql, UPDATE_TASK_STATUS_SQL, status, orderId, userId, productId);
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableUserMessageQueueTask::UpdateTaskStatus m_conn == nullptr");
        return false;
    }
    if (!m_conn->ExecuteUpdate(szUpdateTaskStatusSql))
    {
        LOG_WARN("CTableUserMessageQueueTask::UpdateTaskStatus ExecuteUpdate fail");
        return false;
    }
    LOG_DEBUG("Out CTableUserMessageQueueTask::UpdateTaskStatus");
    return true;
}

bool CTableUserMessageQueueTask::Insert(const UMQTaskRecord &record)
{
    LOG_DEBUG("In CTableUserMessageQueueTask::Insert");
    if (!m_conn)
    {
        LOG_WARN("CTableUserMessageQueueTask::Insert m_conn == nullptr");
        return false;
    }
    char szInsertSql[300];
    sprintf(szInsertSql, INSERT_SQL, record.orderId.c_str(), record.messageOffset.c_str(), record.status.c_str(), record.orderId.c_str()
    , record.productId.c_str(), record.userId.c_str(),
            record.shopId.c_str(), record.amount.c_str(), record.totalMoney.c_str());

    if (!m_conn->ExecuteInsert(szInsertSql))
    {
        LOG_WARN("CTableUserMessageQueueTask::Insert ExecuteInsert fail");
        return false;
    }

    LOG_DEBUG("Out CTableUserMessageQueueTask::Insert");
    return true;
}

bool CTableUserMessageQueueTask::QueryByStatus(const int status, vector<UMQTaskRecord *> &records)
{
    LOG_DEBUG("In CTableUserMessageQueueTask::QueryByStatus status:%d", status);
    char szSelectSql[300];
    sprintf(szSelectSql, SELECT_BY_STATUS_SQL, status);
    bool result = true;
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableUserMessageQueueTask::QueryByStatus m_conn == nullptr");
        return false;
    }
    if (m_conn->ExecuteQuery(szSelectSql))
    {
        while (m_conn->Featch())
        {
            UMQTaskRecord *record = new UMQTaskRecord();
            m_conn->GetField(0, record->umsgTaskId);
            m_conn->GetField(1, record->orderId);
            m_conn->GetField(2, record->productId);
            m_conn->GetField(3, record->userId);
            m_conn->GetField(4, record->shopId);
            m_conn->GetField(5, record->amount);
            m_conn->GetField(6, record->totalMoney);
            m_conn->GetField(7, record->messageOffset);
            m_conn->GetField(8, record->status);
            records.push_back(record);
        }
    }
    else
    {
        LOG_WARN("CTableUserMessageQueueTask::Insert ExecuteQuery fail");
        result = false;
    }
    m_conn->FreeResult();
    LOG_DEBUG("Out CTableUserMessageQueueTask::QueryByStatus");
    return result;
}