#include "../../include/messagequeue/CMsgProducer.h"
#include "../../include/log/log.h"

CMsgProducer::CMsgProducer(int id, string topic, string brokers) : m_id(id), m_strTopic(topic), m_strBrokers(brokers)
{
    LOG_DEBUG("In CMsgProducer::CMsgProducer id:%d, topic:%s, brokers:%s", id, topic.c_str(), brokers.c_str());
    m_pConf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    m_pTopicConf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    m_iPartition = RdKafka::Topic::PARTITION_UA;
    m_iStartOffset = RdKafka::Topic::OFFSET_BEGINNING;
    m_pConf->set("metadata.broker.list", m_strBrokers, m_strErr);
    m_pConf->set("event_cb", &m_exEventCb, m_strErr);
    m_pConf->set("dr_cb", &m_exDrCb, m_strErr);
    m_pConf->set("default_topic_conf", m_pTopicConf, m_strErr);
    m_pProducer = RdKafka::Producer::create(m_pConf, m_strErr);
    m_iPartition = 0;
    if (!m_pProducer)
    {
        LOG_WARN("Failed to create producer: %s", m_strErr.c_str());
    }
    else
    {
        LOG_DEBUG("% Created producer %s", m_pProducer->name());
    }
    LOG_DEBUG("Out CMsgProducer::CMsgProducer id:%d, topic:%s, brokers:%s", id, topic.c_str(), brokers.c_str());
}

CMsgProducer::~CMsgProducer()
{
    LOG_DEBUG("In CMsgProducer::CMsgProducer id:%d, topic:%s, brokers:%s", m_id, m_strTopic.c_str(), m_strBrokers.c_str());
    LOG_DEBUG("Out CMsgProducer::CMsgProducer id:%d, topic:%s, brokers:%s", m_id, m_strTopic.c_str(), m_strBrokers.c_str());
}

bool CMsgProducer::send(string message)
{
    LOG_DEBUG("In CMsgProducer::send id:%d, topic:%s, brokers:%s", m_id, m_strTopic.c_str(), m_strBrokers.c_str());
    LOG_DEBUG("CMsgProducer::send message:%s", message.c_str());

    int isPersisted = 0;
    RdKafka::ErrorCode resp =
        m_pProducer->produce(m_strTopic, m_iPartition,
                             RdKafka::Producer::RK_MSG_COPY,
                             const_cast<char *>(message.c_str()), message.size(),
                             NULL, 0,
                             0,
                             m_pHeaders,
                             &isPersisted);
    m_pProducer->poll(1000);
    LOG_DEBUG("Out CMsgProducer::send isPersisted:%d", isPersisted);

    return isPersisted;
}