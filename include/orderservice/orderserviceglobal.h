#ifndef GLOBAL_H
#define GLOBAL_H
#include "../thread/CThreadPool.h"
#include "../orderservice/CServer.h"
#include "../log/log.h"
#include "../ipc/sharemutex.h"

extern CThreadPool orderThreadPool;
extern CServer globalServer;
extern CShareMutex globalShareMutex;

class COrderGlobalObj
{
public:
    COrderGlobalObj()
    {
        LOG_INIT("log/orderservice", "orderserivce", 5);
    }
};

#endif