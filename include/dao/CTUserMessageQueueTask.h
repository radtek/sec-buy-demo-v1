#ifndef CTABLE_USER_MESSAGE_QUEUE_TASKH
#define CTABLE_USER_MESSAGE_QUEUE_TASKH
#include "CDiskDB.h"
#include <vector>
#include <string>
using namespace std;

#define UPDATE_TASK_STATUS_SQL "update user_message_queue_task set status = %d where order_id = % d and user_id = %d and product_id = %d"
#define INSERT_SQL "insert into user_message_queue_task( umsg_task_id, message_offset, status,order_id, product_id, user_id, shop_id, amount, total_money) values(%s, %s, %s, %s, %s, %s, %s, %s, %s)"
#define SELECT_BY_STATUS_SQL "select * from user_message_queue_task where status = %d for update"
typedef struct UMQTaskRecord
{
    string umsgTaskId;
    string orderId;
    string productId;
    string userId;
    string shopId;
    string amount;
    string totalMoney;
    string messageOffset;
    string status;
}UMQTaskRecord;

class CTableUserMessageQueueTask
{
private:
    CDiskDB *m_conn;


public:
    CTableUserMessageQueueTask(CDiskDB *pConn);
    bool UpdateTaskStatus(const int status, const int orderId, const int userId, const int productId);
    bool QueryByStatus(const int status, vector<UMQTaskRecord *> &records);
    bool Insert(const UMQTaskRecord &record);
};

#endif