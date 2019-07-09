#include "../../include/thread/CThreadPool.h"
#include "../../include/log/log.h"

//静态成员初始化
vector<CThreadTask *> CThreadPool::m_vecTaskList;
map<string, TaskStatus *> CThreadPool::m_mapTaskStatus;
bool CThreadPool::shutdown = false;
pthread_mutex_t CThreadPool::m_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CThreadPool::m_pthreadCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t CThreadPool::m_mapTaskStatusMutex = PTHREAD_MUTEX_INITIALIZER;

//线程管理类构造函数
CThreadPool::CThreadPool(int threadNum)
{
    LOG_DEBUG("In CThreadPool::CThreadPool threadNum:%d", threadNum);
    this->m_threadNum = threadNum;
    Create();
    LOG_DEBUG("Out CThreadPool::CThreadPool threadNum:%d", threadNum);
}

//线程回调函数
void *CThreadPool::ThreadFunc(void *threadData)
{
    LOG_DEBUG("In CThreadPool::ThreadFunc");
    LOG_DEBUG("CThreadPool::ThreadFunc tpthread created");
    pthread_t tid = pthread_self();
    CMsgProducer *pProducer = new CMsgProducer(1, "secBuy", "localhost");
loop:
    LOG_DEBUG("CThreadPool::ThreadFunc loop:");
    try
    {
        while (1)
        {
            pthread_mutex_lock(&m_pthreadMutex);
            //如果队列为空，等待新任务进入任务队列
            while (m_vecTaskList.size() == 0 && !shutdown)
            {
                LOG_DEBUG("CThreadPool::ThreadFunc wait task");
                pthread_cond_wait(&m_pthreadCond, &m_pthreadMutex);
                LOG_DEBUG("CThreadPool::ThreadFunc got task");
            }

            //关闭线程
            if (shutdown)
            {
                pthread_mutex_unlock(&m_pthreadMutex);
                LOG_INFO("CThreadPool::ThreadFunc pthread exit");
                pthread_exit(NULL);
            }

            LOG_DEBUG("CThreadPool::ThreadFunc pthread run");
            vector<CThreadTask *>::iterator iter = m_vecTaskList.begin();
            //取出一个任务并处理之
            CThreadTask *task = *iter;
            if (iter != m_vecTaskList.end())
            {
                task = *iter;
                m_vecTaskList.erase(iter);
            }

            pthread_mutex_unlock(&m_pthreadMutex);

            task->Run(pProducer);
            delete task;

            LOG_DEBUG("CThreadPool::ThreadFunc pthread idle");
        }
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
    }
    goto loop;

    return (void *)0;
}

//往任务队列里添加任务并发出线程同步信号
bool CThreadPool::AddTask(CThreadTask *task)
{
    LOG_DEBUG("In CThreadPool::CThreadPool AddTask task id:%s!", task->m_id.c_str());
    pthread_mutex_lock(&m_pthreadMutex);
    m_vecTaskList.push_back(task);
    pthread_mutex_unlock(&m_pthreadMutex);
    pthread_cond_signal(&m_pthreadCond);
    LOG_DEBUG("Out CThreadPool::CThreadPool AddTasktask id:%s!", task->m_id.c_str());
    return true;
}

//创建线程
bool CThreadPool::Create()
{
    LOG_DEBUG("In CThreadPool::Create!");
    pthread_id = new pthread_t[m_threadNum];
    for (int i = 0; i < m_threadNum; i++)
    {
        pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);
        LOG_DEBUG("CThreadPool::Create pthread_create:%lu", pthread_id[i]);
    }
    LOG_DEBUG("Out CThreadPool::Create!");
    return true;
}

//停止所有线程
bool CThreadPool::Quit()
{
    LOG_INFO("In CThreadPool::Quit");
    //避免重复调用
    if (shutdown)
        return false;
    LOG_INFO("In CThreadPool::Quit Now I will end all threads!");

    //唤醒所有等待进程，线程池也要销毁了
    shutdown = true;
    pthread_cond_broadcast(&m_pthreadCond);

    //清楚僵尸
    for (int i = 0; i < m_threadNum; i++)
        pthread_join(pthread_id[i], NULL);

    delete[] pthread_id;
    pthread_id = NULL;

    //销毁互斥量和条件变量
    pthread_mutex_destroy(&m_pthreadMutex);
    pthread_cond_destroy(&m_pthreadCond);
    LOG_INFO("Out CThreadPool::Quit");
    return true;
}

//获取当前队列中的任务数
int CThreadPool::getTaskSize()
{
    return m_vecTaskList.size();
}