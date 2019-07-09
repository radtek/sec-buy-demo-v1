
#ifndef CREDIS_H
#define CREDIS_H
#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>

#include <hiredis/hiredis.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

class CRedis
{
public:
  int m_id;
  CRedis();
  CRedis(int id);
  ~CRedis();
  bool connect(std::string host, int port);
  void Close();
  std::string get(std::string key);
  bool set(std::string key, std::string value);
  bool setnx(std::string key, std::string value);
  bool setnx(std::string key, std::string value, int expire);
  bool expire(std::string key, int seconds);
  bool deletekey(std::string key);
  bool getset(std::string key, std::string value, std::string &gotValue);
  long int get_current_micros(void);
  long int inc(std::string key);
  /*V1
    一个问题在于每次提交一个Redis请求，如果执行完第一条命令后应用异常或者重启，锁将无法过期，
    一种改善方案就是使用Lua脚本（包含SETNX和EXPIRE两条命令），
    但是如果Redis仅执行了一条命令后crash或者发生主从切换，依然会出现锁没有过期时间，最终导致无法释放
    */
  bool tryLockV1();

  /*V2
1. 在锁竞争较高的情况下，会出现Value不断被覆盖，但是没有一个Client获取到锁
2. 在获取锁的过程中不断的修改原有锁的数据，设想一种场景C1，C2竞争锁，C1获取到了锁，
C2锁执行了GETSET操作修改了C1锁的过期时间，如果C1没有正确释放锁，锁的过期时间被延长，
其它Client需要等待更久的时间
*/
  bool tryLockV2();
  bool tryLockV3();

private:
  redisContext *m_connect;
  redisReply *m_reply;
};

#endif
