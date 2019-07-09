#include "../../include/dao/CTableBuyOrder.h"
#include "../../include/log/log.h"

CTableBuyOrder::CTableBuyOrder(CDiskDB *conn) : m_conn(conn)
{
    if (conn)
    {
        LOG_DEBUG("In CTableBuyOrder::CTableBuyOrder conn id:%d", conn->m_id);
    }
    else
    {
        LOG_WARN("In CTableBuyOrder::CTableBuyOrder conn is empty");
    }
}

bool CTableBuyOrder::QueryByOrderId(const int orderId, BuyOrderRecord &record)
{
    LOG_DEBUG("In CTableBuyOrder::QueryByOrderId orderId:%d", orderId);
    char szSelectSql[300];
    sprintf(szSelectSql, SELECT_BUY_ORDER_SQL, orderId);
    bool result = true;
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableBuyOrder::QueryByOrderId m_conn == nullptr");
        return false;
    }
    if (m_conn->ExecuteQuery(szSelectSql))
    {
        if (m_conn->Featch())
        {
            m_conn->GetField(0, record.orderId);
            m_conn->GetField(1, record.productId);
            m_conn->GetField(2, record.userId);
            m_conn->GetField(3, record.shopId);
            m_conn->GetField(4, record.amount);
            m_conn->GetField(5, record.totalMoney);
        }
    }
    else
    {
        LOG_WARN("CTableBuyOrder::QueryByOrderId ExecuteQuery fail");
        result = false;
    }
    m_conn->FreeResult();
    LOG_DEBUG("Out CTableBuyOrder::QueryByOrderId");
    return result;
}

bool CTableBuyOrder::DeleteByOrderId(const int orderId)
{
    LOG_DEBUG("In CTableBuyOrder::DeleteByOrderId orderId:%d", orderId);
    char szDeletcSql[300];
    sprintf(szDeletcSql, DELETE_BUY_ORDER_SQL, orderId);
    bool result = true;
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableBuyOrder::DeleteByOrderId m_conn == nullptr");
        return false;
    }
    if (!m_conn->ExecuteDelete(szDeletcSql))
    {
        LOG_WARN("CTableBuyOrder::DeleteByOrderId ExecuteDelete fail");
        result = false;
    }
    LOG_DEBUG("Out CTableBuyOrder::DeleteByOrderId");
    return result;
}

bool CTableBuyOrder::Insert(const BuyOrderRecord &record)
{
    LOG_DEBUG("In CTableBuyOrder::Insert orderId:%s", record.orderId.c_str());
    if (!m_conn)
    {
        LOG_WARN("CTableBuyOrder::Insert m_conn == nullptr");
        return false;
    }
    char szInsertSql[300];
    sprintf(szInsertSql, INSERT_BUY_ORDER_SQL, record.orderId.c_str(), record.productId.c_str(), record.userId.c_str(),
            record.shopId.c_str(), record.amount.c_str(), record.totalMoney.c_str());

    if (!m_conn->ExecuteInsert(szInsertSql))
    {
        LOG_WARN("CTableBuyOrder::Insert ExecuteInsert fail");
        return false;
    }
    LOG_DEBUG("Out CTableBuyOrder::Insert");
    return true;
}