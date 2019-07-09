#ifndef CTABLE_BUY_ORDER_H
#define CTABLE_BUY_ORDER_H
#include "CDiskDB.h"
#include <vector>
#include <string>
using namespace std;
#define SELECT_BUY_ORDER_SQL "select * from buy_order where order_id = %d"
#define INSERT_BUY_ORDER_SQL "insert into buy_order(order_id, product_id, user_id, shop_id, amount, total_money) values(%s, %s ,%s ,%s ,%s ,%s)"
#define DELETE_BUY_ORDER_SQL "delete from buy_order where order_id = %d"

typedef struct BuyOrderRecord
{
    string orderId;
    string productId;
    string userId;
    string shopId;
    string amount;
    string totalMoney;
} BuyOrderRecord;

class CTableBuyOrder
{
private:
    CDiskDB *m_conn;

public:
    CTableBuyOrder(CDiskDB *conn);
    bool QueryByOrderId(const int orderId, BuyOrderRecord &record);
    bool Insert(const BuyOrderRecord &record);
    bool DeleteByOrderId(const int orderId);
};

#endif