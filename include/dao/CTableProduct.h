#ifndef CTABLE_PRODUCT_H
#define CTABLE_PRODUCT_H
#include "CDiskDB.h"
#include <vector>
#include <string>
using namespace std;
#define SELECT_PRODUCT_SQL "select * from product where product_id = %d for update"


typedef struct ProductRecord
{
    string productID;
    string productName;
    string price;
} ProductRecord;

class CTableProduct
{
private:
    CDiskDB *m_conn;

public:
    CTableProduct(CDiskDB *pConn);
    bool QueryByProductID(const int userID, ProductRecord &record);

};

#endif