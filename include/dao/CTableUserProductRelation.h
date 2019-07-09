#ifndef CTABLE_USER_PRODUCT_RELATION_H
#define CTABLE_USER_PRODUCT_RELATION_H
#include "CDiskDB.h"
#include <vector>
#include <string>
using namespace std;

#define SELECT_USER_PRODUCT_RELATION_SQL "select * from user_product_relation where user_id = %d and product_id = %d for update"
#define UPDATE_USER_PRODUCT_RELATION_AMOUNT_SQL "update user_product_relation set amount = %d where relation_id = %d"
#define INSERT_USER_PRODUCT_RELATION_SQL "insert into user_product_relation( relation_id, user_id, product_id, amount) values(%s, %s, %s, %s)"
typedef struct UserProductRelationRecord
{
        string relationId;
        string productId;
        string userId;
        string amount;
} UserProductRelationRecord;

class CTableUserProductRelation
{
private:
    CDiskDB *m_conn;

public:
    CTableUserProductRelation(CDiskDB *pConn);
    bool QueryByProductIdAndUserId(const int userID, const int productID,UserProductRelationRecord &record);
    bool UpdateAmountByRelationId(const int amount, const int relationId);
    bool Insert(const UserProductRelationRecord &record);
};

#endif