#include "../../include/threadtask/CAcceptClientTask.h"
#include "../../include/orderservice/orderserviceglobal.h"
bool CAcceptClientTask::Run(CMsgProducer *pProducer)
{
    LOG_DEBUG("In CAcceptClientTask Run");
    bool result = true;
    try
    {
        do
        {
            AcceptClintTaskData *data = (AcceptClintTaskData *)m_ptrData;
            if (data == nullptr || data->len <= 0)
            {
                LOG_DEBUG("CAcceptClientTask Run m_ptrData == null or data->len <= 0");
                result = false;
                break;
            }
            int index = 0;
            int len = data->len;
            CEventLoop &eventLoop = globalServer.m_eventLoop;
            int *szFd = data->szFd;
            int fd = -1;
            for (; index < len; ++index)
            {
                fd = *(szFd+index);
                if (eventLoop.createFileEvent(fd, AE_READABLE, CTcpAcceptedClientSocket::RecvHandler, nullptr) > 0)
                {
                    globalServer.AddClient(fd);
                    LOG_DEBUG("CServer AcceptHandler createFileEvent success index:%d fd:%d!", index, fd);
                }
                else
                {
                    LOG_WARN("CServer AcceptHandler close cliFd:%d", fd);
                    close(fd);
                }
            }
        } while (0);
    }
    catch (exception &e)
    {
        LOG_WARN("CAcceptClientTask Run catch");
        LOG_WARN("CAcceptClientTask Run exception:%s", e.what());
        result = false;
    }

    LOG_DEBUG("Out CAcceptClientTask Run result:%d", result);
    return result;
}

CAcceptClientTask::~CAcceptClientTask() {
    if(m_ptrData != nullptr) {
        free(m_ptrData);
        m_ptrData = nullptr;
    }
}
