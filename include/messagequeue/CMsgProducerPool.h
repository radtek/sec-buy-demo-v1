#ifndef CMESSAGE_PRODUCER_POOL_H
#define CMESSAGE_PRODUCER_POOL_H
#include <list>
#include <string>
#include <pthread.h>
#include "CMsgProducer.h"

using namespace std;

class CMsgProducerPool
{
  public:
    ~CMsgProducerPool();
    void InitMsgProducerPool(int maxSize);
    CMsgProducer *GetMsgProducer();
    void ReleaseMsgProducer(CMsgProducer *);
    static CMsgProducerPool *GetInstance();

  private:
    CMsgProducerPool(int maxSize);
    CMsgProducer *CreateMsgProducer(int id);
    void CreateMsgProducerPool(int size);
    void DestoryMsgProducer(CMsgProducer *producer);
    void DestoryMsgProducerPool();
    CMsgProducerPool();

  private:
    int m_curSize;
    int m_maxSize;
    string m_user;
    string m_password;
    string m_IP;
    int m_port;
    list<CMsgProducer *> m_producerList;
    pthread_cond_t m_cond; 
    pthread_mutex_t m_lock;
    static CMsgProducerPool *m_producerPoll;
};
#endif