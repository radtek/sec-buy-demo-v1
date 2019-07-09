#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../include/messagequeue/CKafka.h"
#include "../../include/dao/CDiskDBConnPool.h"
#include "../../include/threadtask/CUserConsumerProcessTask.h"
#include "../../include/threadtask/CClearProcessedTaskStatus.h"
#include "../../include/thread/CThreadPool.h"
#include "../../include/userservice/userserviceglobal.h"
#include "../../include/dao/CTUserMessageQueueTask.h"
#include "../../include/dao/CTableUserMessageQueueInfo.h"
#include "../../include/log/log.h"
#include "../../include/idservice/idhelper.h"

#define USER_MSG_QUEUE_INFO_ID 0

int getConsumerNextOffset()
{
    LOG_DEBUG("In main getConsumerNextOffset");
    int offset = 0;
    UMQInfo record;
    CDiskDBConnPool *connPool = CDiskDBConnPool::GetInstance();
    CDiskDB *conn = connPool->GetConnection();
    CTableUserMessageQueueInfo tableUmsgqInfo(conn);
    int umsgqId = USER_MSG_QUEUE_INFO_ID;
    LOG_DEBUG("main getConsumerNextOffset umsgqId:%d", umsgqId);
    if (tableUmsgqInfo.QueryByUmsgqId(umsgqId, record))
    {
        LOG_DEBUG("main getConsumerNextOffset QueryByUmsgqId success");
       // sleep(300);
        offset = std::stoi(record.acceptedOffset);
        ++offset;
    }
    else
    {
        LOG_DEBUG("main getConsumerNextOffset QueryByUmsgqId fail");
        record.umsgsId = "0";
        record.acceptedOffset = "0";
        if (tableUmsgqInfo.Insert(record))
        {
            LOG_DEBUG("main getConsumerNextOffset Insert success");
        }
    }
    conn->TransationCommit();
    LOG_DEBUG("Out main getConsumerNextOffset offset:%d", offset);
    return offset;
}

vector<string> split(const string &str, const string &delim)
{
    LOG_DEBUG("In main split str:%s delim:%s", str.c_str(), delim.c_str());
    vector<string> res;
    if ("" == str)
        return res;
    //先将要切割的字符串从string类型转换为char*类型
    char *strs = new char[str.length() + 1]; //不要忘了
    strcpy(strs, str.c_str());

    char *d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char *p = strtok(strs, d);
    while (p)
    {
        string s = p;     //分割得到的字符串转换为string类型
        res.push_back(s); //存入结果数组
        p = strtok(NULL, d);
    }
    LOG_DEBUG("Out main split res.size:%d", res.size());
    return res;
}

void consumer()
{
    LOG_DEBUG("In main consumer");
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    int32_t partition = RdKafka::Topic::PARTITION_UA;
    int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;
    std::string topic_str = "secBuy";
    std::string brokers = "localhost";
    std::string errstr;
    RdKafka::Conf::ConfResult res;
    conf->set("metadata.broker.list", brokers, errstr);
    ExampleEventCb ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);

    conf->set("enable.partition.eof", "true", errstr);
    RdKafka::Consumer *consumer = RdKafka::Consumer::create(conf, errstr);
    if (!consumer)
    {
        //std::cerr << "Failed to create consumer: " << errstr << std::endl;
        LOG_ERROR("Failed to create consumer:%s", errstr.c_str());
        exit(1);
    }

    //std::cout << "% Created consumer " << consumer->name() << std::endl;
    LOG_DEBUG("% Created consumer :%s", consumer->name().c_str());

    RdKafka::Topic *topic = RdKafka::Topic::create(consumer, topic_str,
                                                   tconf, errstr);
    if (!topic)
    {
        //std::cerr << "Failed to create topic: " << errstr << std::endl;
        LOG_ERROR("Failed to create topic:%s", errstr.c_str());
        exit(1);
    }

    partition = 0;
    getConsumerNextOffset();
    //start_offset = getConsumerNextOffset();
    //start_offset = 0;
    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);
    if (resp != RdKafka::ERR_NO_ERROR)
    {
        //std::cerr << "Failed to start consumer: " << RdKafka::err2str(resp) << std::endl;
        LOG_ERROR("Failed to start consumer:%s", RdKafka::err2str(resp).c_str());
        exit(1);
    }

    ExampleConsumeCb ex_consume_cb;
    int index = 0;
    CDiskDBConnPool *connPool = CDiskDBConnPool::GetInstance();
    CDiskDB *conn = connPool->GetConnection();
    while (true)
    {
        try
        {
            conn->TransationStart();
            index++;
            RdKafka::Message *msg = consumer->consume(topic, partition, -1);
            //msg_consume(msg, NULL);
            if (msg->err() == RdKafka::ERR_NO_ERROR)
            {
                //std::cout << "msg offset:%d " << msg->offset() << std::endl;
                //std::cout << "msg payload:%s " << (char *)msg->payload() << std::endl;
                LOG_DEBUG("msg offset:%d", msg->offset());
                LOG_DEBUG("msg payload:%s", (char *)msg->payload());
                string message((char *)msg->payload());
                string strToken = ",";
                std::vector<std::string> record = split(message, strToken);

                UMQTaskRecord taskRecord;
                enum idTypeEnum idType = user_message_queue_task_id;
                unsigned long umsgTaskId = getId(idType);

                taskRecord.umsgTaskId = std::to_string(umsgTaskId);
                taskRecord.orderId = record[0];
                taskRecord.productId = record[1];
                taskRecord.userId = record[2];
                taskRecord.shopId = record[3];
                taskRecord.amount = record[4];
                taskRecord.totalMoney = record[5];
                taskRecord.messageOffset = std::to_string((int)msg->offset());
                taskRecord.status = "0";

                CTableUserMessageQueueTask tableTask(conn);
                tableTask.Insert(taskRecord);

                CTableUserMessageQueueInfo tableUmsgqInfo(conn);

                int umsgqId = 0;
                int offset = (int)msg->offset();
                tableUmsgqInfo.UpdateAcceptedOffsetByUmsgqId(offset, umsgqId);

                CUserConsumerProcessTask *userConsumerTask = new CUserConsumerProcessTask("CUserConsumerProcessTask", taskRecord.umsgTaskId);
                UserProcessTask *taskData = (UserProcessTask *)malloc(sizeof(struct UserProcessTask));



                taskData->iUmsgTaskId = umsgTaskId;
                taskData->iMessageOffset = msg->offset();
                taskData->iStatus = 0;
                taskData->iOrderId = std::stoi(record[0].c_str());
                taskData->iProductId = std::stoi(record[1].c_str());
                taskData->iUserId = std::stoi(record[2].c_str());
                taskData->iShopId = std::stoi(record[3].c_str());
                taskData->iAmount = std::stoi(record[4].c_str());
                taskData->iTotalMoney = std::stof(record[5].c_str());

                TaskStatus *taskStatus = new TaskStatus();
                taskStatus->status = 0;
                string id("test");
                taskStatus->id = record[0].c_str();

                pthread_mutex_lock(&CThreadPool::m_mapTaskStatusMutex);
                CThreadPool::m_mapTaskStatus.insert(pair<string, TaskStatus *>(taskStatus->id, taskStatus));
                pthread_mutex_unlock(&CThreadPool::m_mapTaskStatusMutex);

                userConsumerTask->setData(taskData);

                consumerThreadPool.AddTask(userConsumerTask);

                CClearProcessedTaskStatus *clearTask = new CClearProcessedTaskStatus("CClearProcessedTaskStatus", "");
                consumerThreadPool.AddTask(clearTask);
                conn->TransationCommit();
            }
        }
        catch (exception &e)
        {
            cout << e.what() << endl;
            conn->TransationRollback();
        }
        consumer->poll(0);
    }
    LOG_DEBUG("Out main consumer");
}

int main()
{
    LOG_INIT("log/userservice", "userserivce", 5);
    LOG_DEBUG("In user service main");
    consumer();
    LOG_DEBUG("Out user service main");
    return 0;
}