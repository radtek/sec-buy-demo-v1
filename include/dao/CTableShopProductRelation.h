#ifndef CTABLE_SHOP_PRODUCT_RELATION_H
#define CTABLE_SHOP_PRODUCT_RELATION_H
#include "CDiskDB.h"
#include <vector>
#include <string>
using namespace std;

#define SELECT_SHOP_PRODUCT_RELATION_SQL "select * from shop_product_relation where shop_id = %d and product_id = %d for update"


typedef struct ShopProductRelationRecord
{
        string relationId;
        string productId;
        string shopId;
        string amount;
} ShopProductRelationRecord;

class CTableShopProductRelation
{
private:
    CDiskDB *m_conn;

public:
    CTableShopProductRelation(CDiskDB *pConn);
    bool QueryByProductIdAndShopId(const int shopID, const int productID,ShopProductRelationRecord &record);

};

#endif