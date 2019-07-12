#ifndef GLOBAL_H
#define GLOBAL_H
#include "../thread/CThreadPool.h"
#include "../orderservice/CServer.h"
#include "../log/log.h"
#include "../ipc/sharemutex.h"
#include "../config/CConfig.h"
extern CThreadPool orderThreadPool;
extern CServer globalServer;
extern CConfig globalConfig;
extern CShareMutex globalShareMutex;

class COrderGlobalObj
{
public:
    COrderGlobalObj()
    {
        LOG_INIT("log/orderservice", "orderserivce", 5);
        //config = new CConfig();
        //threadPool = new CThreadPool(config->m_threadNum);
        //server = new CServer(config->m_IP, config->m_port, config->m_maxClientNumber);
    }
    //CConfig config;
    //CServer *server;
    //CThreadPool *threadPool;
};

#endif