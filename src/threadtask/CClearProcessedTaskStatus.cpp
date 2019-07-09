#include "../../include/threadtask/CClearProcessedTaskStatus.h"
#include "../../include/log/log.h"

bool CClearProcessedTaskStatus::Run(CMsgProducer *pProducer)
{
    LOG_DEBUG("In CClearProcessedTaskStatus::Run");
    bool result = true;
    pthread_mutex_lock(&CThreadPool::m_mapTaskStatusMutex);
    try
    {
        map<string, TaskStatus *>::iterator it;
        for (it = CThreadPool::m_mapTaskStatus.begin(); it != CThreadPool::m_mapTaskStatus.end(); ++it)
        {
            LOG_DEBUG("CClearProcessedTaskStatus::Run task id:%s, status%d", ((it->second)->id).c_str(),(it->second)->status);
            if ((it->second)->status)
            {

                free(it->second);
                CThreadPool::m_mapTaskStatus.erase(it);
            }
        }
    }
    catch (exception &e)
    {
        LOG_WARN("CClearProcessedTaskStatus::Run catch");
        LOG_WARN("CClearProcessedTaskStatus::Run exception:%s", e.what());
        result = false;
    }
    pthread_mutex_unlock(&CThreadPool::m_mapTaskStatusMutex);
    LOG_DEBUG("Out CClearProcessedTaskStatus::Run result:%d", result);
    return result;
}
