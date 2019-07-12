
#include "../../include/orderservice/orderserviceglobal.h"

COrderGlobalObj orderGlobalObj;
CConfig globalConfig;
CThreadPool orderThreadPool(globalConfig.m_threadNum);
CServer globalServer(globalConfig.m_IP, globalConfig.m_port, globalConfig.m_maxClientNumber);
//CShareMutex globalShareMutex("accept.lock");
