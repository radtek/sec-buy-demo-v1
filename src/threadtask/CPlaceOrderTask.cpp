#include "../../include/threadtask/CPlaceOrderTask.h"
#include "../../include/orderservice/orderserviceglobal.h"
#include "../../include/log/log.h"
#include "../../include/idservice/idhelper.h"
#include "../../include/dao/CTableProduct.h"
bool CPlaceOrderTask::Run(CMsgProducer *pProducer)
{
    bool result = true;
    CDiskDBConnPool *connPool = CDiskDBConnPool::GetInstance();
    CDiskDB *conn = connPool->GetConnection();

    if (conn == nullptr)
    {
        LOG_WARN("Out CPlaceOrderTask::Run conn is empty");
        return false;
    }
    Order *order = (Order *)m_ptrData;
    try
    {
        do
        {
            conn->TransationStart();
            LOG_DEBUG("CPlaceOrderTask::Run productID:%d number:%d shopid:%d, userID:%d\n",
                 order->productID, order->number, order->shopID, order->userID);

            CTableProduct tableProduct(conn);
            ProductRecord productRecord;
            unsigned long price = 0;

            if (!tableProduct.QueryByProductID(order->productID, productRecord))
            {
                LOG_DEBUG("CPlaceOrderTask::Run QueryByProductID fail!");
                result = false;
                break;
            }
            price = std::stol(productRecord.price);
            unsigned long totalMoney = order->number * price;

            CTableBuyOrder tableOrder(conn);
            BuyOrderRecord buyOrderRecord;

            enum idTypeEnum idType = order_id;
            unsigned long orderId = getId(idType);
            buyOrderRecord.orderId = std::to_string(orderId);
            buyOrderRecord.productId = std::to_string(order->productID);
            buyOrderRecord.userId = std::to_string(order->userID);
            buyOrderRecord.shopId = std::to_string(order->shopID);
            buyOrderRecord.amount = std::to_string(order->number);
            buyOrderRecord.totalMoney = std::to_string(price);
            if (!tableOrder.Insert(buyOrderRecord))
            {
                LOG_DEBUG("CPlaceOrderTask::Run insert fail!");
                result = false;
                break;
            }

            string message = buyOrderRecord.orderId + "," + buyOrderRecord.productId + "," + buyOrderRecord.userId + "," + buyOrderRecord.shopId + "," + buyOrderRecord.amount + "," + buyOrderRecord.totalMoney;
            if (!pProducer->send(message))
            {
                LOG_DEBUG("CPlaceOrderTask::Run produce messag fail!");
                result = false;
                break;
            }
            conn->TransationCommit();
        } while (false);
        if(!result){
            conn->TransationRollback();
        }
    }
    catch (exception &e)
    {
        LOG_WARN("CPlaceOrderTask::Run catch");
        LOG_WARN("CPlaceOrderTask::Run exception:%s", e.what());
        conn->TransationRollback();
        result = false;
    }

    connPool->ReleaseConnection(conn);
    

    LOG_WARN("Out CPlaceOrderTask::Run result:%d", result);
    return result;
}
