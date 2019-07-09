
#include "../../include/orderservice/orderserviceglobal.h"

COrderGlobalObj orderGlobalObj;
CThreadPool orderThreadPool(5);
CServer globalServer("127.0.0.1", 12345, 1100000);
//CShareMutex globalShareMutex("accept.lock");
