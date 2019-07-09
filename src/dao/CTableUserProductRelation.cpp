#include "../../include/dao/CTableUserProductRelation.h"
#include "../../include/log/log.h"

CTableUserProductRelation::CTableUserProductRelation(CDiskDB *conn) : m_conn(conn)
{
    if (conn)
    {
        LOG_DEBUG("In CTableUserProductRelation::CTableUserProductRelation conn id:%d", conn->m_id);
    }
    else
    {
        LOG_WARN("In CTableUserProductRelation::CTableUserProductRelation conn is empty");
    }
}

bool CTableUserProductRelation::QueryByProductIdAndUserId(const int userID, const int productID, UserProductRelationRecord &record)
{
    LOG_DEBUG("In CTableUserProductRelation::QueryByProductIdAndUserId userID:%d, productID:%d", userID, productID);
    char szSelectUserProductRelationSql[300];
    sprintf(szSelectUserProductRelationSql, SELECT_USER_PRODUCT_RELATION_SQL, userID, productID);
    bool result = true;
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableUserProductRelation::QueryByProductIdAndUserId m_conn == nullptr");
        return false;
    }
    if (m_conn->ExecuteQuery(szSelectUserProductRelationSql))
    {
        if (m_conn->Featch())
        {
            m_conn->GetField(0, record.relationId);
            m_conn->GetField(1, record.productId);
            m_conn->GetField(2, record.userId);
            m_conn->GetField(3, record.amount);
        }else {
           result = false; 
           LOG_WARN("CTableUserProductRelation::QueryByProductIdAndUserId Featch fail");
        }
    }
    else
    {
        LOG_WARN("CTableUserProductRelation::QueryByProductIdAndUserId ExecuteQuery fail");
        result = false;
    }
    m_conn->FreeResult();
    LOG_DEBUG("Out CTableUserProductRelation::QueryByProductIdAndUserId");
    return result;
}

bool CTableUserProductRelation::UpdateAmountByRelationId(const int amount, const int relationId)
{
    LOG_DEBUG("In CTableUserProductRelation::UpdateAmountByRelationId amount:%d, relationId:%d", amount, relationId);
    char szUpdateAmountSql[300];
    sprintf(szUpdateAmountSql, UPDATE_USER_PRODUCT_RELATION_AMOUNT_SQL, amount, relationId);
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableUserProductRelation::UpdateAmountByRelationId m_conn == nullptr");
        return false;
    }
    if (!m_conn->ExecuteUpdate(szUpdateAmountSql))
    {
        LOG_WARN("CTableUserProductRelation::UpdateAmountByRelationId ExecuteQuery fail");
        return false;
    }
    LOG_DEBUG("Out CTableUserProductRelation::UpdateAmountByRelationId");
    return true;
}

bool CTableUserProductRelation::Insert(const UserProductRelationRecord &record)
{
    LOG_DEBUG("In CTableUserProductRelation::Insert");
    if (!m_conn)
    {
        LOG_WARN("CTableUserProductRelation::Insert m_conn == nullptr");
        return false;
    }
    char szInsertSql[300];
    sprintf(szInsertSql, INSERT_USER_PRODUCT_RELATION_SQL, record.relationId.c_str(), record.userId.c_str(), record.productId.c_str(), record.amount.c_str());

    if (!m_conn->ExecuteInsert(szInsertSql))
    {
        LOG_WARN("CTableUserProductRelation::Insert ExecuteInsert fail");
        return false;
    }

    LOG_DEBUG("Out CTableUserProductRelation::Insert");
    return true;
}