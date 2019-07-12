#ifndef CCONFIG_H
#define CCONFIG_H
#include <string>
#include <iostream>
#include <fstream>
#include <mysql/mysql.h>
#include <stack>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;
#define DEFAULT_CONFIG_PATH "./orderservice.cfg"
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 54321
#define DEFAULT_MAX_CLIENT_NUM 10000
#define DEFAULT_ACCEPT_CLIENT_BATCH_NUM 10000
#define DEFAULT_CLEAR_CLIENT_BATCH_NUM 10000
#define DEFAULT_EVENT_POLL_WAIT_TIME 1
#define DEFAULT_CLEAR_CLIENT_INTERVAL 60
#define DEFAULT_ORDER_THREAD_NUM 1
class CConfig
{
public:
    CConfig(string filePath = DEFAULT_CONFIG_PATH);
    void loadConfig(string filePath);
    string m_filePath;
    string m_IP;
    int m_port;
    int m_maxClientNumber;
    int m_acceptClientBatchNumber;
    int m_clearClientBatchNumber;
    int m_clearClientInterval;
    int m_eventPollWaitTime;
    int m_threadNum;

private:
    map<string, string> m_mapConfigInfo;
};

#endif