#ifndef CTABLE_USER_MESSAGE_QUEUE_INFO_H
#define CTABLE_USER_MESSAGE_QUEUE_INFO_H
#include "CDiskDB.h"
#include <vector>
#include <string>
using namespace std;
#define USER_MSG_QUEUE_INFO_SELECT_SQL "select * from user_message_queue_info where umsgq_id = %d for update"
#define USER_MSG_QUEUE_INFO_UPDATE_SQL "update user_message_queue_info set accepted_offset = %d where umsgq_id = %d"
#define USER_MSG_QUEUE_INFO_INSERT_SQL "insert into user_message_queue_info( umsgq_id, accepted_offset) values(%s, %s)"


typedef struct UMQInfo
{
    string umsgsId;
    string acceptedOffset;
} UMQInfo;

class CTableUserMessageQueueInfo
{
private:
    CDiskDB *m_conn;

public:
    CTableUserMessageQueueInfo(CDiskDB *pConn);
    bool QueryByUmsgqId(const int umsgqId, UMQInfo &record);
    bool UpdateAcceptedOffsetByUmsgqId(const int acceptedOffset, const int umsgqId);
    bool Insert(const UMQInfo &record);
};


#endif