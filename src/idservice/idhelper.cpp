#include "../../include/idservice/idhelper.h"
#include "../../include/memdb/CMemDBConnectionPool.h"
#include "../../include/log/log.h"

#include <errno.h>
#include <unistd.h>//access, getpid
#include <assert.h>//assert
#include <stdarg.h>//va_list
#include <sys/stat.h>//mkdir
#include <sys/syscall.h>//system call

char szIdTypeName[6][30] = {
    "order_id",
    "user_product_relation_id",
    "user_message_queue_task_id",
    "shop_message_queue_task_id",
    "user_message_queue_info_id",
    "shop_message_queue_info_id"
};

unsigned long getId(int idType)
{
    LOG_DEBUG("In getId");
    CMemDBConnectionPool *memConnPool = CMemDBConnectionPool::GetInstance();
    CRedis *conn = memConnPool->GetConnection();
    if(conn == nullptr) {
        //sleep(3000);
        return 0;
    }

    unsigned long id = 0;

    time_t rawtime;
    struct tm *ptminfo;
    char idName[40] = {0};
    char idRet[40] = {0};

    time(&rawtime);
    ptminfo = localtime(&rawtime);
    sprintf(idName, "%s:%d%02d%02d", szIdTypeName[idType], ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday);
    LOG_DEBUG("idName:%s\n", idName);
    int index = conn->inc(idName);
    if (index > 0)
    {
        sprintf(idRet, "%d%02d%02d%d", ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,index);
        id = std::stol(idRet);
    }
    LOG_DEBUG("getId %ld\n", id);
    memConnPool->ReleaseConnection(conn);
    LOG_DEBUG("Out getId");

    return id;
}
