#include "../../include/memdb/CMemDBConnectionPool.h"
#include "../../include/log/log.h"

CMemDBConnectionPool *CMemDBConnectionPool::m_connPoll = NULL;

CMemDBConnectionPool::CMemDBConnectionPool()
{
    LOG_DEBUG("In CMemDBConnectionPool::CMemDBConnectionPool");
    LOG_DEBUG("Out CMemDBConnectionPool::CMemDBConnectionPool");
}

void CMemDBConnectionPool::InitConnectionPool(string url, string user, string password, int maxSize)
{
    LOG_DEBUG("In CMemDBConnectionPool::InitConnectionPool url:%s, user:%s, password:%s, maxSize:%d", url.c_str(), user.c_str(), password.c_str(), maxSize);
    m_curSize = 0;
    m_maxSize = maxSize;
    m_user = user;
    m_password = password;
    m_lock = PTHREAD_MUTEX_INITIALIZER;
    m_cond = PTHREAD_COND_INITIALIZER;
    CreateConnectionPool(m_maxSize / 2);
    LOG_DEBUG("Out CMemDBConnectionPool::InitConnectionPool");
}

CMemDBConnectionPool::CMemDBConnectionPool(string IP, string user, string password, int maxSize)
{
    LOG_DEBUG("In CMemDBConnectionPool::CMemDBConnectionPool url:%s, user:%s, password:%s, maxSize:%d", IP.c_str(), user.c_str(), password.c_str(), maxSize);
    m_curSize = 0;
    m_maxSize = maxSize;
    m_user = user;
    m_password = password;
    m_IP = IP;
    m_lock = PTHREAD_MUTEX_INITIALIZER;
    m_cond = PTHREAD_COND_INITIALIZER;
    CreateConnectionPool(m_maxSize / 2);
    LOG_DEBUG("Out CMemDBConnectionPool::CMemDBConnectionPool");
}

CMemDBConnectionPool *CMemDBConnectionPool::GetInstance()
{
    LOG_DEBUG("In CMemDBConnectionPool::GetInstance");
    if (m_connPoll == NULL)
    {
        LOG_DEBUG("CMemDBConnectionPool::GetInstance m_connPoll == NULL");
        m_connPoll = new CMemDBConnectionPool("127.0.0.1", "root", "Guangzhou08", 10);
    }
    LOG_DEBUG("Out CMemDBConnectionPool::GetInstance");
    return m_connPoll;
}

void CMemDBConnectionPool::CreateConnectionPool(int num)
{

    CRedis *conn = NULL;
    pthread_mutex_lock(&m_lock);
    for (int i = 0; i < num; ++i)
    {
        conn = CreateConnection();
        if (conn)
        {
            LOG_DEBUG("CMemDBConnectionPool::CreateConnectionPool index:%d Created", i);
            m_connList.push_back(conn);
            ++m_curSize;
        }
    }
    pthread_mutex_unlock(&m_lock);
    LOG_DEBUG("Out CMemDBConnectionPool::CreateConnectionPool");
}

CRedis *CMemDBConnectionPool::CreateConnection()
{
    LOG_DEBUG("In CMemDBConnectionPool::CreateConnection");
    CRedis *conn = NULL;
    conn = new CRedis(m_curSize);

    conn->connect("127.0.0.1", 6379);
    LOG_DEBUG("Out CMemDBConnectionPool::CreateConnection");
    return conn;
}

CRedis *CMemDBConnectionPool::GetConnection()
{
    LOG_DEBUG("In CMemDBConnectionPool::GetConnection");
    CRedis *conn = NULL;
    pthread_mutex_lock(&m_lock);
    int tryTimes = 0;
loop:
    LOG_DEBUG("CMemDBConnectionPool::GetConnection tryTimes:%d", tryTimes);
    LOG_DEBUG("CMemDBConnectionPool::GetConnection m_connList.size():%d", m_connList.size());
    if (m_connList.size() > 0)
    {
        LOG_DEBUG("CMemDBConnectionPool::GetConnection m_connList.size:%d > 0 ", m_connList.size());
        conn = m_connList.front();
        m_connList.pop_front();
    }
    else
    {
        LOG_DEBUG("CMemDBConnectionPool::GetConnection m_connList.size is 0");
        if (m_curSize < m_maxSize)
        {
            conn = CreateConnection();
            if (conn)
            {
                int id = conn->m_id;
                LOG_DEBUG("CMemDBConnectionPool::GetConnection create conn id:%d", id);
                ++m_curSize;
            }
            else
            {
                LOG_WARN("CMemDBConnectionPool::GetConnection create conn fail");
            }
        }
        else
        {
            LOG_WARN("CMemDBConnectionPool::GetConnection can not get conn for m_connList's size reach maxsize");
        }

        if (conn == nullptr)
        {
            LOG_DEBUG("CDiskDBConnPool::GetConnection wait conn");
            pthread_cond_wait(&m_cond, &m_lock);
            LOG_DEBUG("CDiskDBConnPool::GetConnection got conn");
            LOG_DEBUG("CDiskDBConnPool::GetConnection goto loop");
            tryTimes++;
            goto loop;
        }
    }

    pthread_mutex_unlock(&m_lock);
    LOG_DEBUG("Out CMemDBConnectionPool::GetConnection");
    return conn;
}

void CMemDBConnectionPool::ReleaseConnection(CRedis *conn)
{
    LOG_DEBUG("In CMemDBConnectionPool::ReleaseConnection");
    if (conn)
    {
        LOG_DEBUG("CMemDBConnectionPool::ReleaseConnection m_connList.push_back");
        pthread_mutex_lock(&m_lock);
        m_connList.push_back(conn);
        pthread_mutex_unlock(&m_lock);
        pthread_cond_signal(&m_cond);
    }
    LOG_DEBUG("Out CMemDBConnectionPool::ReleaseConnection");
}

CMemDBConnectionPool::~CMemDBConnectionPool()
{
    LOG_DEBUG("In CMemDBConnectionPool::~CMemDBConnectionPool");
    DestoryConnectionPool();
    LOG_DEBUG("Out CMemDBConnectionPool::~CMemDBConnectionPool");
}

void CMemDBConnectionPool::DestoryConnectionPool()
{
    LOG_DEBUG("In CMemDBConnectionPool::DestoryConnectionPool");
    list<CRedis *>::iterator iter;
    pthread_mutex_lock(&m_lock);
    for (iter = m_connList.begin(); iter != m_connList.end(); ++iter)
    {
        DestoryConnection(*iter);
    }

    m_curSize = 0;
    m_connList.clear();
    pthread_mutex_unlock(&m_lock);
    LOG_DEBUG("Out CMemDBConnectionPool::DestoryConnectionPool");
}

void CMemDBConnectionPool::DestoryConnection(CRedis *conn)
{
    LOG_DEBUG("In CMemDBConnectionPool::DestoryConnection id:%d", conn->m_id);
    conn->Close();
    LOG_DEBUG("Out CMemDBConnectionPool::DestoryConnection");
}