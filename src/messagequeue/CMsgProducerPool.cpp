#include "../../include/messagequeue/CMsgProducerPool.h"
#include "../../include/log/log.h"
CMsgProducerPool *CMsgProducerPool::m_producerPoll = NULL;
void CMsgProducerPool::InitMsgProducerPool(int maxSize)
{
    LOG_DEBUG("In CMsgProducerPool::InitMsgProducerPool maxSize:%d", maxSize);
    m_curSize = 0;
    m_maxSize = maxSize;
    pthread_mutex_init(&m_lock, NULL);
    CreateMsgProducerPool(m_maxSize / 2);
    LOG_DEBUG("Out CMsgProducerPool::InitMsgProducerPool");
}

CMsgProducerPool::CMsgProducerPool(int maxSize)
{
    LOG_DEBUG("In CMsgProducerPool::CMsgProducerPool maxSize:%d", maxSize);
    m_curSize = 0;
    m_maxSize = maxSize;
    pthread_mutex_init(&m_lock, NULL);
    CreateMsgProducerPool(m_maxSize / 2);
    LOG_DEBUG("Out CMsgProducerPool::CMsgProducerPool");
}

CMsgProducerPool *CMsgProducerPool::GetInstance()
{
    LOG_DEBUG("In CMsgProducerPool::GetInstance");
    if (m_producerPoll == NULL)
    {
        LOG_DEBUG("CMsgProducerPool::GetInstance m_producerPoll == NULL");
        m_producerPoll = new CMsgProducerPool(10);
    }
    LOG_DEBUG("Out CMsgProducerPool::GetInstance");
    return m_producerPoll;
}

void CMsgProducerPool::CreateMsgProducerPool(int num)
{
    LOG_DEBUG("In CMsgProducerPool::CreateMsgProducerPool");
    num = 1;
    CMsgProducer *produce = NULL;
    pthread_mutex_lock(&m_lock);
    for (int i = 0; i < num; ++i)
    {
        produce = CreateMsgProducer(i);
        if (produce)
        {

            m_producerList.push_back(produce);
            ++m_curSize;
        }
    }
    pthread_mutex_unlock(&m_lock);

    LOG_DEBUG("Out CMsgProducerPool::CreateMsgProducerPool");
}

CMsgProducer *CMsgProducerPool::CreateMsgProducer(int id)
{
    LOG_DEBUG("In CMsgProducerPool::CreateMsgProducer id:%d", id);
    CMsgProducer *produce = new CMsgProducer(id, "test", "localhost");
    LOG_DEBUG("Out CMsgProducerPool::CreateMsgProducer");
    //string message("test gary");
    //produce->send(message);
    return produce;
}

CMsgProducer *CMsgProducerPool::GetMsgProducer()
{
    LOG_DEBUG("In CMsgProducerPool::GetMsgProducer");
    CMsgProducer *producer = NULL;
    int tryTimes = 0;
    pthread_mutex_lock(&m_lock);
loop:
    if (m_producerList.size() > 0)
    {
        LOG_DEBUG("CMsgProducerPool::GetMsgProducer m_producerList.size:%d", m_producerList.size());
        producer = m_producerList.front();
        m_producerList.pop_front();
    }
    else
    {
        LOG_DEBUG("CMsgProducerPool::GetConnection try to create conn");
        if (m_curSize < m_maxSize)
        {
            LOG_DEBUG("CMsgProducerPool::GetMsgProducer m!producer && m_curSize < m_maxSiz");
            producer = CreateMsgProducer(m_curSize);
            if (producer)
            {
                LOG_DEBUG("CMsgProducerPool::GetConnection create conn id:%d", producer->m_id);
                ++m_curSize;
            }
            else
            {
                LOG_WARN("CMsgProducerPool::GetConnection create conn fail");
            }
        }
        else
        {
            LOG_WARN("CMsgProducerPool::GetConnection can not get conn for m_connList's size reach maxsize");
        }

        if (producer == nullptr)
        {
            LOG_DEBUG("CMsgProducerPool::GetConnection wait conn");
            pthread_cond_wait(&m_cond, &m_lock);
            LOG_DEBUG("CMsgProducerPool::GetConnection got conn");
            LOG_DEBUG("CMsgProducerPool::GetConnection goto loop");
            tryTimes++;
            goto loop;
        }
    }

    pthread_mutex_unlock(&m_lock);
    LOG_DEBUG("Out CMsgProducerPool::GetMsgProducer");
    return producer;
}

void CMsgProducerPool::ReleaseMsgProducer(CMsgProducer *producer)
{
    LOG_DEBUG("In CMsgProducerPool::ReleaseMsgProducer");
    if (producer)
    {
        LOG_DEBUG("CMsgProducerPool::ReleaseMsgProducer id:%d", producer->m_id);
        pthread_mutex_lock(&m_lock);
        m_producerList.push_back(producer);

        pthread_mutex_unlock(&m_lock);
    }
    LOG_DEBUG("Out CMsgProducerPool::ReleaseMsgProducer");
}

CMsgProducerPool::~CMsgProducerPool()
{
    LOG_DEBUG("In CMsgProducerPool::~CMsgProducerPool");
    DestoryMsgProducerPool();
    LOG_DEBUG("Out CMsgProducerPool::~CMsgProducerPool");
}

void CMsgProducerPool::DestoryMsgProducerPool()
{
    LOG_DEBUG("In CMsgProducerPool::DestoryMsgProducerPool");
    list<CMsgProducer *>::iterator iter;
    pthread_mutex_lock(&m_lock);
    for (iter = m_producerList.begin(); iter != m_producerList.end(); ++iter)
    {
        DestoryMsgProducer(*iter);
    }

    m_curSize = 0;
    m_producerList.clear();
    pthread_mutex_unlock(&m_lock);
    LOG_DEBUG("Out CMsgProducerPool::DestoryMsgProducerPool");
}

void CMsgProducerPool::DestoryMsgProducer(CMsgProducer *producer)
{
    LOG_DEBUG("In CMsgProducerPool::DestoryMsgProducer");
    LOG_DEBUG("Out CMsgProducerPool::DestoryMsgProducer");
}