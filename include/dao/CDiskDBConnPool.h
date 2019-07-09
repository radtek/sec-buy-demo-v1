#ifndef CDB_CONNECTION_POOL_H
#define CDB_CONNECTION_POOL_H
#include <list>
#include <string>
#include <pthread.h>
#include "CDiskDB.h"

using namespace std;

class CDiskDBConnPool
{
  public:
    ~CDiskDBConnPool();
    void InitConnectionPool(string IP, int port, string user, string password, int maxSize);
    CDiskDB *GetConnection();
    void ReleaseConnection(CDiskDB *);
    static CDiskDBConnPool *GetInstance();

  private:
    CDiskDBConnPool();
    CDiskDB *CreateConnection();
    void CreateConnectionPool(int size);
    void DestoryConnection(CDiskDB *connection);
    void DestoryConnectionPool();
    CDiskDBConnPool(string IP, int port, string user, string password, int maxSize);

  private:
    int m_curSize;
    int m_maxSize;
    string m_user;
    string m_password;
    string m_IP;
    int m_port;
    list<CDiskDB *> m_connList;
    
    pthread_mutex_t m_lock;
    pthread_cond_t m_cond;  
    static CDiskDBConnPool *m_connPoll;
};
#endif