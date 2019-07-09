#ifndef CMESSAGE_PRODUCER_H
#define CMESSAGE_PRODUCER_H
#include <list>
#include <string>
#include <pthread.h>
#include "CKafka.h"

using namespace std;

class CMsgProducer
{
private:
    RdKafka::Conf *m_pConf;
    RdKafka::Conf *m_pTopicConf;
    RdKafka::Producer *m_pProducer;
    RdKafka::Conf::ConfResult res;
    ExampleEventCb m_exEventCb;
    ExampleDeliveryReportCb m_exDrCb;
    RdKafka::Headers *m_pHeaders;

    int32_t m_iPartition;
    int64_t m_iStartOffset;
    string m_strTopic;
    string m_strBrokers;
    string m_strErr;

public:
    ~CMsgProducer();
    CMsgProducer(int id, string topic, string brokers);
    bool send(string message);
    int m_id;
};
#endif