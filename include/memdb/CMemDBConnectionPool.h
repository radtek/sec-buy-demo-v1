#ifndef CCACHE_CONNECTION_POOL_H
#define CCACHE_CONNECTION_POOL_H
#include <list>
#include <string>
#include <pthread.h>
#include "CRedis.h"

using namespace std;

class CMemDBConnectionPool
{
  public:
    ~CMemDBConnectionPool();
    void InitConnectionPool(string url, string user, string password, int maxSize);
    CRedis *GetConnection();
    void ReleaseConnection(CRedis *);
    static CMemDBConnectionPool *GetInstance();

  private:
    CMemDBConnectionPool();
    CRedis *CreateConnection();
    void CreateConnectionPool(int size);
    void DestoryConnection(CRedis *connection);
    void DestoryConnectionPool();
    CMemDBConnectionPool(string url, string user, string password, int maxSize);

  private:
    int m_curSize;
    int m_maxSize;
    string m_user;
    string m_password;
    string m_IP;
    int m_port;
    list<CRedis *> m_connList;
    pthread_cond_t m_cond; 
    pthread_mutex_t m_lock;
    static CMemDBConnectionPool *m_connPoll;
};
#endif