#include "../../include/dao/CTableUserMessageQueueInfo.h"
#include "../../include/log/log.h"

CTableUserMessageQueueInfo::CTableUserMessageQueueInfo(CDiskDB *conn) : m_conn(conn)
{
    if (conn != nullptr)
    {
        LOG_DEBUG("In CTableUserMessageQueueInfo::CTableUserMessageQueueInfo conn id:%d", conn->m_id);
    }
    else
    {
        LOG_WARN("In CTableUserMessageQueueInfo::CTableUserMessageQueueInfo conn is empty");
    }
}

bool CTableUserMessageQueueInfo::QueryByUmsgqId(const int umsgqId, UMQInfo &record)
{

    LOG_DEBUG("In CTableUserMessageQueueInfo::QueryByUmsgqId umsgqId:%d", umsgqId);
    char szSelectSql[300];
    sprintf(szSelectSql, USER_MSG_QUEUE_INFO_SELECT_SQL, umsgqId);
    bool result = false;
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableUserMessageQueueInfo::QueryByUmsgqId m_conn == nullptr");
        return false;
    }
    if (m_conn->ExecuteQuery(szSelectSql))
    {
        if (m_conn->Featch())
        {
            m_conn->GetField(0, record.umsgsId);
            m_conn->GetField(1, record.acceptedOffset);
            result = true;
        }
        m_conn->FreeResult();
    }

    LOG_DEBUG("Out CTableUserMessageQueueInfo::QueryByUmsgqId");
    return result;
}

bool CTableUserMessageQueueInfo::UpdateAcceptedOffsetByUmsgqId(const int acceptedOffset, const int umsgqId)
{
    LOG_DEBUG("In CTableUserMessageQueueInfo::UpdateAcceptedOffsetByUmsgqId umsgqId:%d, acceptedOffset:%d", umsgqId, acceptedOffset);
    char szUpdateSql[300];
    sprintf(szUpdateSql, USER_MSG_QUEUE_INFO_UPDATE_SQL, acceptedOffset, umsgqId);
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableUserMessageQueueInfo::UpdateAcceptedOffsetByUmsgqId m_conn == nullptr");
        return false;
    }
    if (!m_conn->ExecuteUpdate(szUpdateSql))
    {
        LOG_WARN("CTableUserMessageQueueInfo::UpdateAcceptedOffsetByUmsgqId ExecuteQuery fail");
        return false;
    }
    LOG_DEBUG("Out CTableUserMessageQueueInfo::UpdateAcceptedOffsetByUmsgqId");
    return true;
}

bool CTableUserMessageQueueInfo::Insert(const UMQInfo &record)
{
    LOG_DEBUG("In CTableUserMessageQueueInfo::Insert");
    if (!m_conn)
    {
        LOG_WARN("CTableUserMessageQueueInfo::Insert m_conn == nullptr");
        return false;
    }
    char szInsertSql[300];
    sprintf(szInsertSql, USER_MSG_QUEUE_INFO_INSERT_SQL, record.umsgsId.c_str(), record.acceptedOffset.c_str());

    if (!m_conn->ExecuteInsert(szInsertSql))
    {
        LOG_WARN("CTableUserMessageQueueInfo::Insert ExecuteInsert fail");
        return false;
    }

    LOG_DEBUG("Out CTableUserMessageQueueInfo::Insert");
    return true;
}