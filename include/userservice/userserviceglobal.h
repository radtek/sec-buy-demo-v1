#ifndef USER_SERVICE_GLOBAL_H
#define USER_SERVICE_GLOBAL_H
#include "../thread/CThreadPool.h"
#include "../../include/log/log.h"

extern CThreadPool consumerThreadPool;

class CUserGlobalObj
{
public:
    CUserGlobalObj()
    {
        printf("gary_test\n");
        LOG_INIT("log/userservice", "userserivce", 5);
    }
};

#endif