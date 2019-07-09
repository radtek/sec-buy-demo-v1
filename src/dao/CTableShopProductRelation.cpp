#include "../../include/dao/CTableShopProductRelation.h"
#include "../../include/log/log.h"

CTableShopProductRelation::CTableShopProductRelation(CDiskDB *conn) : m_conn(conn)
{
    if (conn)
    {
        LOG_DEBUG("In CTableShopProductRelation::CTableShopProductRelation conn id:%d", conn->m_id);
    }
    else
    {
        LOG_WARN("In CTableShopProductRelation::CTableShopProductRelation conn is empty");
    }
}

bool CTableShopProductRelation::QueryByProductIdAndShopId(const int shopID, const int productID, ShopProductRelationRecord &record)
{
    LOG_DEBUG("In CTableShopProductRelation::QueryByProductIdAndShopId shopID:%d, productID:%d", shopID, productID);
    char szSelectShopProductRelationSql[300];
    sprintf(szSelectShopProductRelationSql, SELECT_SHOP_PRODUCT_RELATION_SQL, shopID, productID);
    bool result = true;
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableShopProductRelation::QueryByProductIdAndShopId m_conn == nullptr");
        return false;
    }
    if (m_conn->ExecuteQuery(szSelectShopProductRelationSql))
    {
        if (m_conn->Featch())
        {
            m_conn->GetField(0, record.relationId);
            m_conn->GetField(1, record.productId);
            m_conn->GetField(2, record.shopId);
            m_conn->GetField(3, record.amount);
        }
    }
    else
    {
        LOG_WARN("CTableShopProductRelation::QueryByProductIdAndShopId ExecuteQuery fail");
        result = false;
    }
    m_conn->FreeResult();
    LOG_DEBUG("Out CTableShopProductRelation::QueryByProductIdAndShopId");
    return result;
}
