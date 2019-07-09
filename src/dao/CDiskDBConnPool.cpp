#include "../../include/dao/CDiskDBConnPool.h"
#include "../../include/log/log.h"
CDiskDBConnPool *CDiskDBConnPool::m_connPoll = NULL;

CDiskDBConnPool::CDiskDBConnPool()
{
    LOG_DEBUG("In CDiskDBConnPool::CDiskDBConnPool");
    LOG_DEBUG("Out CDiskDBConnPool::CDiskDBConnPool");
}

void CDiskDBConnPool::InitConnectionPool(string IP, int port, string username, string password, int maxSize)
{
    LOG_DEBUG("In CDiskDBConnPool::InitConnectionPool IP%s, port:%d, username:%s, password:%s, maxsize:%d",
              IP.c_str(), port, username.c_str(), password.c_str(), maxSize);
    m_curSize = 0;
    m_maxSize = maxSize;
    m_user = username;
    m_password = password;
    m_port = port;
    m_IP = IP;
    m_cond = PTHREAD_COND_INITIALIZER;
    m_lock = PTHREAD_MUTEX_INITIALIZER;
    CreateConnectionPool(m_maxSize / 2);
    LOG_DEBUG("Out CDiskDBConnPool::InitConnectionPool");
}

CDiskDBConnPool::CDiskDBConnPool(string IP, int port, string username, string password, int maxSize)
{
    LOG_DEBUG("In CDiskDBConnPool::CDiskDBConnPool IP%s, port:%d, username:%s, password:%s, maxsize:%d",
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
    LOG_DEBUG("Out CDiskDBConnPool::InitConnectionPool");
}

CDiskDBConnPool *CDiskDBConnPool::GetInstance()
{
    LOG_DEBUG("In CDiskDBConnPool::GetInstance");
    if (m_connPoll == NULL)
    {
        LOG_DEBUG("CDiskDBConnPool::GetInstance m_connPoll == NULL");
        m_connPoll = new CDiskDBConnPool("127.0.0.1", 0, "root", "Guangzhou08", 10);
        LOG_DEBUG("CDiskDBConnPool::GetInstance got m_connPoll");
    }
    LOG_DEBUG("Out CDiskDBConnPool::GetInstance");
    return m_connPoll;
}

void CDiskDBConnPool::CreateConnectionPool(int num)
{
    LOG_DEBUG("In CDiskDBConnPool::CreateConnectionPool num:%d", num);
    CDiskDB *conn = NULL;
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
    LOG_DEBUG("Out CDiskDBConnPool::CreateConnectionPool");
}

CDiskDB *CDiskDBConnPool::CreateConnection()
{
    LOG_DEBUG("In CDiskDBConnPool::CreateConnection");
    CDiskDB *conn = NULL;
    conn = new CDiskDB(m_curSize, m_IP, m_port, m_user, m_password, "mysql");
    conn->AutoCommit(0);
    LOG_DEBUG("Out CDiskDBConnPool::CreateConnection");
    return conn;
}

CDiskDB *CDiskDBConnPool::GetConnection()
{
    LOG_DEBUG("In CDiskDBConnPool::GetConnection");
    CDiskDB *conn = NULL;
    int tryTimes = 0;
    pthread_mutex_lock(&m_lock);
loop:
    LOG_DEBUG("CDiskDBConnPool::GetConnection tryTimes:%d", tryTimes);
    LOG_DEBUG("CDiskDBConnPool::GetConnection m_connList.size():%d", m_connList.size());
    if (m_connList.size() > 0)
    {

        conn = m_connList.front();
        int id = conn->m_id;
        LOG_DEBUG("CDiskDBConnPool::GetConnection got conn id:%d", id);
        m_connList.pop_front();
    }
    else
    {
        LOG_DEBUG("CDiskDBConnPool::GetConnection m_connList's size is 0");
        if (m_curSize < m_maxSize)
        {
            LOG_DEBUG("CDiskDBConnPool::GetConnection try to create conn");
            conn = CreateConnection();
            if (conn)
            {
                int id = conn->m_id;
                LOG_DEBUG("CDiskDBConnPool::GetConnection create conn id:%d", id);
                ++m_curSize;
            }
            else
            {
                LOG_WARN("CDiskDBConnPool::GetConnection create conn fail");
            }
        }
        else
        {
            LOG_WARN("CDiskDBConnPool::GetConnection can not get conn for m_connList's size reach maxsize");
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

    LOG_DEBUG("Out CDiskDBConnPool::GetConnection");
    return conn;
}

void CDiskDBConnPool::ReleaseConnection(CDiskDB *conn)
{
    LOG_DEBUG("In CDiskDBConnPool::ReleaseConnection");
    if (conn)
    {
        pthread_mutex_lock(&m_lock);
        LOG_DEBUG("CDiskDBConnPool::ReleaseConnection m_connList push_back");
        m_connList.push_back(conn);

        pthread_mutex_unlock(&m_lock);
        pthread_cond_signal(&m_cond);
    }
    LOG_DEBUG("Out CDiskDBConnPool::ReleaseConnection");
}

CDiskDBConnPool::~CDiskDBConnPool()
{
    LOG_DEBUG("In CDiskDBConnPool::~CDiskDBConnPool");
    DestoryConnectionPool();
    LOG_DEBUG("Out CDiskDBConnPool::~CDiskDBConnPool");
}

void CDiskDBConnPool::DestoryConnectionPool()
{
    LOG_DEBUG("In CDiskDBConnPool::DestoryConnectionPool");
    list<CDiskDB *>::iterator iter;
    pthread_mutex_lock(&m_lock);
    for (iter = m_connList.begin(); iter != m_connList.end(); ++iter)
    {

        DestoryConnection(*iter);
    }

    m_curSize = 0;
    m_connList.clear();
    pthread_mutex_unlock(&m_lock);
    LOG_DEBUG("Out CDiskDBConnPool::DestoryConnectionPool");
}

void CDiskDBConnPool::DestoryConnection(CDiskDB *conn)
{
    LOG_DEBUG("In CDiskDBConnPool::DestoryConnection");
    conn->Close();
    LOG_DEBUG("Out CDiskDBConnPool::DestoryConnection");
}