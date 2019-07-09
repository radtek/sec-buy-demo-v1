#include "../../include/thread/CThreadTask.h"
#include "../../include/log/log.h"
void CThreadTask::setData(void* data) {
    LOG_DEBUG("In CThreadTask::setData");
    m_ptrData = data;
    LOG_DEBUG("Out CThreadTask::setData");
}

