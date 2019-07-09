#ifndef CCONNECTION_POOL_H
#define CCONNECTION_POOL_H
#include <list>
#include <string>
#include <pthread.h>
#include "../../include/log/log.h"


using namespace std;

template <class T> class CConnectionPool
{
  public:
    CConnectionPool(string IP, int port, string user, string password, int maxSize);
    ~CConnectionPool();
    T *GetConnection();
    void ReleaseConnection(T *);

  private:
    CConnectionPool();
    T *CreateConnection();
    void CreateConnectionPool(int size);
    void DestoryConnection(T *connection);
    void DestoryConnectionPool();
    

  private:
    int m_curSize;
    int m_maxSize;
    string m_user;
    string m_password;
    string m_IP;
    int m_port;
    list<T *> m_connList;
    
    pthread_mutex_t m_lock;
    pthread_cond_t m_cond;  
};



/* template <class  T>  CConnectionPool<T>::CConnectionPool()
{
    LOG_DEBUG("In CConnectionPool CConnectionPool");
    LOG_DEBUG("Out template <class  T>  CConnectionPool<T>::CConnectionPool");
}

template <class  T>  CConnectionPool<T>::CConnectionPool(string IP, int port, string username, string password, int maxSize)
{
    LOG_DEBUG("In CConnectionPool CConnectionPool IP%s, port:%d, username:%s, password:%s, maxsize:%d",
              IP.c_str(), port, username.c_str(), password.c_str(), maxSize);
    m_curSize = 0;
    m_maxSize = maxSize;
    m_user = username;
    m_password = password;
    m_IP = IP;
    m_port = port;
    // pthread_mutex_init(&m_lock, NULL);
    m_cond = PTHREAD_COND_INITIALIZER;
    m_lock = PTHREAD_MUTEX_INITIALIZER;
    CreateConnectionPool(m_maxSize / 2);
    LOG_DEBUG("Out template <class  T>  CConnectionPool<T>::InitConnectionPool");
}

template <class  T> void CConnectionPool<T>::CreateConnectionPool(int num)
{
    LOG_DEBUG("In CConnectionPool CreateConnectionPool num:%d", num);
    T *conn = NULL;
    pthread_mutex_lock(&m_lock);
    for (int i = 0; i < num; ++i)
    {
        conn = CreateConnection();
        if (conn)
        {
            m_connList.push_back(conn);
            ++m_curSize;
        }
    }
    pthread_mutex_unlock(&m_lock);
    LOG_DEBUG("Out template <class  T>  CConnectionPool<T>::CreateConnectionPool");
}

template <class  T> T * CConnectionPool<T>::CreateConnection()
{
    LOG_DEBUG("In CConnectionPool CreateConnection");
    T *conn = NULL;
    conn = new T(m_curSize, m_IP, m_port, m_user, m_password, "mysql");
    conn->AutoCommit(0);
    LOG_DEBUG("Out template <class  T>  CConnectionPool<T>::CreateConnection");
    return conn;
}

template <class  T> T * CConnectionPool<T>::GetConnection()
{
    LOG_DEBUG("In CConnectionPool GetConnection");
    T *conn = NULL;
    int tryTimes = 0;
    pthread_mutex_lock(&m_lock);
loop:
    LOG_DEBUG("template <class  T>  CConnectionPool<T>::GetConnection tryTimes:%d", tryTimes);
    LOG_DEBUG("template <class  T>  CConnectionPool<T>::GetConnection m_connList.size():%d", m_connList.size());
    if (m_connList.size() > 0)
    {

        conn = m_connList.front();
        int id = conn->m_id;
        LOG_DEBUG("template <class  T>  CConnectionPool<T>::GetConnection got conn id:%d", id);
        m_connList.pop_front();
    }
    else
    {
        LOG_DEBUG("template <class  T>  CConnectionPool<T>::GetConnection m_connList's size is 0");
        if (m_curSize < m_maxSize)
        {
            LOG_DEBUG("template <class  T>  CConnectionPool<T>::GetConnection try to create conn");
            conn = CreateConnection();
            if (conn)
            {
                int id = conn->m_id;
                LOG_DEBUG("template <class  T>  CConnectionPool<T>::GetConnection create conn id:%d", id);
                ++m_curSize;
            }
            else
            {
                LOG_WARN("template <class  T>  CConnectionPool<T>::GetConnection create conn fail");
            }
        }
        else
        {
            LOG_WARN("template <class  T>  CConnectionPool<T>::GetConnection can not get conn for m_connList's size reach maxsize");
        }

        if (conn == nullptr)
        {
            LOG_DEBUG("template <class  T>  CConnectionPool<T>::GetConnection wait conn");
            pthread_cond_wait(&m_cond, &m_lock);
            LOG_DEBUG("template <class  T>  CConnectionPool<T>::GetConnection got conn");
            LOG_DEBUG("template <class  T>  CConnectionPool<T>::GetConnection goto loop");
            tryTimes++;
            goto loop;
        }
    }

    pthread_mutex_unlock(&m_lock);

    LOG_DEBUG("Out template <class  T>  CConnectionPool<T>::GetConnection");
    return conn;
}

template <class  T> void  CConnectionPool<T>::ReleaseConnection(T *conn)
{
    LOG_DEBUG("In CConnectionPool ReleaseConnection");
    if (conn)
    {
        pthread_mutex_lock(&m_lock);
        LOG_DEBUG("template <class  T>  CConnectionPool<T>::ReleaseConnection m_connList push_back");
        m_connList.push_back(conn);

        pthread_mutex_unlock(&m_lock);
        pthread_cond_signal(&m_cond);
    }
    LOG_DEBUG("Out template <class  T>  CConnectionPool<T>::ReleaseConnection");
}

template <class  T>  CConnectionPool<T>::~CConnectionPool()
{
    LOG_DEBUG("In CConnectionPool ~CConnectionPool");
    DestoryConnectionPool();
    LOG_DEBUG("Out template <class  T>  CConnectionPool<T>::~CConnectionPool");
}

template <class  T> void  CConnectionPool<T>::DestoryConnectionPool()
{
    LOG_DEBUG("In CConnectionPool DestoryConnectionPool");
    //list<T *>::iterator iter;
    pthread_mutex_lock(&m_lock);
    
    for (; m_connList.size(); )
    {
        T * conn = m_connList.front();
        DestoryConnection(conn);
        m_connList.pop_front();
    }

    m_curSize = 0;
    m_connList.clear();
    pthread_mutex_unlock(&m_lock);
    LOG_DEBUG("Out template <class  T>  CConnectionPool<T>::DestoryConnectionPool");
}

template <class  T> void  CConnectionPool<T>::DestoryConnection(T *conn)
{
    LOG_DEBUG("In CConnectionPool DestoryConnection");
    conn->Close();
    LOG_DEBUG("Out template <class  T>  CConnectionPool<T>::DestoryConnection");
} */
#endif