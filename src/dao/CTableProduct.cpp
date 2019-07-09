#include "../../include/dao/CTableProduct.h"
#include "../../include/log/log.h"

CTableProduct::CTableProduct(CDiskDB *conn) : m_conn(conn)
{
    if (conn)
    {
        LOG_DEBUG("In CTableProduct::CTableProduct conn id:%d", conn->m_id);
    }
    else
    {
        LOG_WARN("In CTableProduct::CTableProduct conn is empty");
    }
}

bool CTableProduct::QueryByProductID(const int productId, ProductRecord &record)
{
    LOG_DEBUG("In CTableProduct::QueryByProductId productId:%d", productId);
    char szSelectProductSql[300];
    sprintf(szSelectProductSql, SELECT_PRODUCT_SQL, productId);
    bool result = true;
    if (m_conn == nullptr)
    {
        LOG_WARN("CTableProduct::QueryByUserID m_conn == nullptr");
        return false;
    }
    if (m_conn->ExecuteQuery(szSelectProductSql))
    {
        if (m_conn->Featch())
        {
            m_conn->GetField(0, record.productID);
            m_conn->GetField(1, record.productName);
            m_conn->GetField(2, record.price);
        }
    }
    else
    {
        LOG_WARN("CTableProduct::QueryByUserID ExecuteQuery fail");
        result = false;
    }
    m_conn->FreeResult();
    LOG_DEBUG("Out CTableProduct::QueryByUserID");
    return result;
}