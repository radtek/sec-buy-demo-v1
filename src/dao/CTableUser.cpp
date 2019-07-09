#include "../../include/dao/CTableUser.h"
#include "../../include/log/log.h"

CTableUser::CTableUser(CDiskDB *conn) : m_conn(conn)
{
    if (conn)
    {
        LOG_DEBUG("In CTableUser::CTableUser conn id:%d", conn->m_id);
    }
    else
    {
        LOG_WARN("In CTableUser::CTableUser conn is empty");
    }
}

bool CTableUser::QueryByUserID(const int userId, UserRecord &record)
{
    LOG_DEBUG("In CTableUser::QueryByOrderId userId:%d", userId);
    char szSelectUserSql[300];
    sprintf(szSelectUserSql, SELECT_USER_SQL, userId);
    bool result = true;
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableUser::QueryByUserID m_conn == nullptr");
        return false;
    }
    if (m_conn->ExecuteQuery(szSelectUserSql))
    {
        if (m_conn->Featch())
        {
            m_conn->GetField(0, record.userID);
            m_conn->GetField(1, record.userName);
            m_conn->GetField(2, record.money);
        }
    }
    else
    {
        LOG_WARN("CTableUser::QueryByUserID ExecuteQuery fail");
        result = false;
    }
    m_conn->FreeResult();
    LOG_DEBUG("Out CTableUser::QueryByUserID");
    return result;
}

bool CTableUser::UpdateMoneyByUserID(const float money, const int userID)
{

    LOG_DEBUG("Out CTableUser::UpdateMoneyByUserID money:%f, userid:%d", money, userID);
    char szUpdateUserMoneySql[300];
    sprintf(szUpdateUserMoneySql, UPDATE_USER_MOMEY_SQL, money, userID);
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableUser::UpdateMoneyByUserID m_conn == nullptr");
        return false;
    }
    if (!m_conn->ExecuteUpdate(szUpdateUserMoneySql))
    {
        LOG_WARN("CTableUser::UpdateMoneyByUserID ExecuteUpdate fail");
        return false;
    }
    LOG_DEBUG("Out CTableUser::UpdateMoneyByUserID");
    return true;
}