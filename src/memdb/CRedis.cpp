#include "../../include/memdb/CRedis.h"
#include "../../include/log/log.h"

std::string g_szRedisType[7] = {
    "gary",
    "REDIS_REPLY_STATUS",
    "REDIS_REPLY_ERROR",
    "REDIS_REPLY_INTEGER",
    "REDIS_REPLY_NIL",
    "REDIS_REPLY_STRING",
    "REDIS_REPLY_ARRAY"};


    CRedis::CRedis() {}

    CRedis::CRedis(int id):m_id(id) {}

    CRedis::~CRedis()
    {
        this->m_connect = NULL;
        this->m_reply = NULL;
    }
    void CRedis::Close() {}

    bool CRedis::connect(std::string host, int port)
    {
        this->m_connect = redisConnect(host.c_str(), port);
        if (this->m_connect != NULL && this->m_connect->err)
        {
            printf("connect error: %s\n", this->m_connect->errstr);
            return 0;
        }
        return 1;
    }

    std::string CRedis::get(std::string key)
    {
        printf("**************cmd begin**************\n");
        printf("cmd:GET %s\n", key.c_str());
        this->m_reply = (redisReply *)redisCommand(this->m_connect, "GET %s", key.c_str());
        std::string str = this->m_reply->str;
        freeReplyObject(this->m_reply);
        printf("******************cmd end**************\n");

        return str;
    }

    bool CRedis::set(std::string key, std::string value)
    {
        bool result = true;
        std::string cmd = "SET " + key + " " + value;
        printf("cmd:%s\n", cmd.c_str());
        this->m_reply = (redisReply *)redisCommand(this->m_connect, cmd.c_str());
        printf("this->m_reply->type:%d, %s\n", this->m_reply->type, g_szRedisType[this->m_reply->type].c_str());
        if (this->m_reply->type == REDIS_REPLY_NIL)
        {
            result = false;
        }
        freeReplyObject(this->m_reply);
        printf("result:%d\n", result);
        return result;
    }

    bool CRedis::setnx(std::string key, std::string value)
    {
        bool result = true;
        std::string cmd = "SETNX " + key + " " + value;
        printf("**************cmd begin**************\n");
        printf("cmd:%s\n", cmd.c_str());
        this->m_reply = (redisReply *)redisCommand(this->m_connect, cmd.c_str());
        printf("this->m_reply->type:%d, %s\n", this->m_reply->type, g_szRedisType[this->m_reply->type].c_str());
        if (this->m_reply->type == REDIS_REPLY_INTEGER && this->m_reply->integer == 0)
        {
            result = false;
        }
        freeReplyObject(this->m_reply);
        printf("result:%d\n", result);
        printf("******************cmd end**************\n");
        return result;
    }

    bool CRedis::setnx(std::string key, std::string value, int expire)
    {
        bool result = true;
        std::string cmd = "SETNX " + key + " " + value + std::to_string(expire);
        printf("**************cmd begin**************\n");
        printf("cmd:%s\n", cmd.c_str());
        this->m_reply = (redisReply *)redisCommand(this->m_connect, cmd.c_str());
        printf("this->m_reply->type:%d, %s\n", this->m_reply->type, g_szRedisType[this->m_reply->type].c_str());
        if (this->m_reply->type == REDIS_REPLY_INTEGER && this->m_reply->integer == 0)
        {
            result = false;
        }
        freeReplyObject(this->m_reply);
        printf("result:%d\n", result);
        printf("******************cmd end**************\n");
        return result;
    }

    bool CRedis::expire(std::string key, int seconds)
    {
        bool result = true;
        printf("**************cmd begin**************\n");
        printf("cmd:EXPIRE %s %d", key.c_str(), seconds);
        this->m_reply = (redisReply *)redisCommand(this->m_connect, "EXPIRE %s %d", key.c_str(), seconds);
        printf("this->m_reply->type:%d, %s\n", this->m_reply->type, g_szRedisType[this->m_reply->type].c_str());
        if (this->m_reply->type == REDIS_REPLY_INTEGER && this->m_reply->integer == 0)
        {
            result = false;
        }
        freeReplyObject(this->m_reply);
        printf("result:%d\n", result);
        printf("******************cmd end**************\n");
        return result;
    }

    bool CRedis::deletekey(std::string key)
    {
        bool result = true;
        std::string cmd = "DEL " + key;
        printf("**************cmd begin**************\n");
        printf("cmd:%s\n", cmd.c_str());
        this->m_reply = (redisReply *)redisCommand(this->m_connect, cmd.c_str());
        printf("this->m_reply->type:%d, %s\n", this->m_reply->type, g_szRedisType[this->m_reply->type].c_str());
        if (this->m_reply->type == REDIS_REPLY_INTEGER && this->m_reply->integer == 0)
        {
            result = false;
        }
        freeReplyObject(this->m_reply);
        printf("result:%d\n", result);
        printf("******************cmd end**************\n");
        return result;
    }

    bool CRedis::getset(std::string key, std::string value, std::string &gotValue)
    {
        printf("**************cmd begin**************\n");
        bool result = true;
        std::string cmd = "GETSET " + key + " " + value;
        printf("cmd:%s\n", cmd.c_str());
        this->m_reply = (redisReply *)redisCommand(this->m_connect, cmd.c_str());
        printf("this->m_reply->type:%d, %s\n", this->m_reply->type, g_szRedisType[this->m_reply->type].c_str());
        if (this->m_reply->type == REDIS_REPLY_NIL)
        {
            result = false;
        } else {
            gotValue = this->m_reply->str;
        }
        freeReplyObject(this->m_reply);
        printf("result:%d\n", result);
        printf("******************cmd end**************\n");
        return result;
    }

    long int CRedis::get_current_micros(void)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (long int)tv.tv_sec * 1000 * 1000 + tv.tv_usec;
    }

    long int CRedis::inc(std::string key){
        LOG_DEBUG("**************cmd begin**************\n");
        int result = 0;
        std::string cmd = "INCR " + key;
        LOG_DEBUG("cmd:%s\n", cmd.c_str());
        this->m_reply = (redisReply *)redisCommand(this->m_connect, cmd.c_str());
        LOG_DEBUG("this->m_reply->type:%d, %s\n", this->m_reply->type, g_szRedisType[this->m_reply->type].c_str());
        if (this->m_reply->type == REDIS_REPLY_NIL)
        {
            result = -1;
        } else {
            result = this->m_reply->integer;
        }
        freeReplyObject(this->m_reply);
        LOG_DEBUG("result:%d\n", result);
        LOG_DEBUG("******************cmd end**************\n");
        return result;
    }

    /*V1
    一个问题在于每次提交一个Redis请求，如果执行完第一条命令后应用异常或者重启，锁将无法过期，
    一种改善方案就是使用Lua脚本（包含SETNX和EXPIRE两条命令），
    但是如果Redis仅执行了一条命令后crash或者发生主从切换，依然会出现锁没有过期时间，最终导致无法释放
    */
    bool CRedis::tryLockV1()
    {
        std::string lockName("tryLockV1");
        std::string lockValue("simpleLock");
        if (this->setnx(lockName, lockValue))
        {
            this->expire(lockName, 3);
            return true;
        }
        else
        {
            return false;
        }
    }


/*V2
1. 在锁竞争较高的情况下，会出现Value不断被覆盖，但是没有一个Client获取到锁
2. 在获取锁的过程中不断的修改原有锁的数据，设想一种场景C1，C2竞争锁，C1获取到了锁，
C2锁执行了GETSET操作修改了C1锁的过期时间，如果C1没有正确释放锁，锁的过期时间被延长，
其它Client需要等待更久的时间
*/
    bool CRedis::tryLockV2()
    {
        long int currentTime = get_current_micros();
        long int newExpireTime = currentTime + 3*1000*1000;        
        printf("currentTime:  %ld\nnewExpireTime:%ld\n", currentTime, newExpireTime);
        std::string lockName("tryLockV2");

        if (!this->setnx(lockName, std::to_string(newExpireTime)))
        {
            std::string strOldExpireTime = this->get(lockName);
            long oldExpireTime = std::stol(strOldExpireTime);
            printf("oldExpireTime:%ld\n", oldExpireTime);
            if(oldExpireTime < currentTime) {
                printf("oldExpireTime < currentTime\n");
                std::string strCurrentExpireTime("");
                if(this->getset(lockName, std::to_string(newExpireTime),strCurrentExpireTime)) {
                    long int currentExpireTime = std::stol(strCurrentExpireTime);
                    printf("currentExpireTime:%ld\n", currentExpireTime);
                    if(oldExpireTime == currentExpireTime) {
                        return true;
                    }
                }
            }else{
                printf("oldExpireTime >= currentTime\n");
            }

        }
        return false;   
    }

    bool CRedis::tryLockV3()
    {
        std::string lockName("tryLockV3");
        std::string lockValue("simpleLock");
        if (this->setnx(lockName, lockValue, 3))
        {
            return true;
        }
        else
        {
            return false;
        }  
    }

