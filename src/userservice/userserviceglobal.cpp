#include "../../include/userservice/userserviceglobal.h"
#include "../../include/connectionpool/CConnectionPool.h"
#include "../../include/dao/CDiskDB.h"

CUserGlobalObj userGlobalObj;
CThreadPool consumerThreadPool(5);
CConnectionPool<CDiskDB> connectionPool("127.0.0.1", 0, "root", "Guangzhou08", 10);
