#ifndef CTABLE_USER_H
#define CTABLE_USER_H
#include "CDiskDB.h"
#include <vector>
#include <string>
using namespace std;
#define SELECT_USER_SQL "select * from user where user_id = %d for update"
#define UPDATE_USER_MOMEY_SQL "update user set money = %f where user_id = %d"

typedef struct UserRecord
{
    string userID;
    string userName;
    string money;
} UserRecord;

class CTableUser
{
private:
    CDiskDB *m_conn;

public:
    CTableUser(CDiskDB *pConn);
    bool QueryByUserID(const int userID, UserRecord &record);
    bool UpdateMoneyByUserID(const float money, const int userID);
};

#endif