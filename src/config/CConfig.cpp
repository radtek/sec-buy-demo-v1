#include "../../include/config/CConfig.h"

CConfig::CConfig(string filePath) : m_filePath(filePath), m_IP(DEFAULT_IP), m_port(DEFAULT_PORT),
                                                          m_maxClientNumber(DEFAULT_MAX_CLIENT_NUM),
                                                          m_acceptClientBatchNumber(DEFAULT_ACCEPT_CLIENT_BATCH_NUM),
                                                          m_clearClientBatchNumber(DEFAULT_CLEAR_CLIENT_BATCH_NUM),
                                                          m_eventPollWaitTime(DEFAULT_EVENT_POLL_WAIT_TIME),
                                                          m_clearClientInterval(DEFAULT_CLEAR_CLIENT_INTERVAL),
                                                          m_threadNum(DEFAULT_ORDER_THREAD_NUM)
{
    loadConfig(m_filePath);
}

void CConfig::loadConfig(string filePath)
{
    ifstream configFile;
    string path = filePath;
    configFile.open(path.c_str());
    string str_line;

    if (configFile.is_open())
    {
        while (!configFile.eof())
        {
            getline(configFile, str_line);
            if (str_line.find('#') == 0) //过滤掉注释信息，即如果首个字符为#就过滤掉这一行
            {
                continue;
            }
            size_t pos = str_line.find('=');
            string str_key = str_line.substr(0, pos);
            string str_value = str_line.substr(pos + 1);
            m_mapConfigInfo.insert(pair<string, string>(str_key, str_value));
        }
        map<string, string>::iterator cofigItem;
        cofigItem = m_mapConfigInfo.find("IP");
        if (cofigItem != m_mapConfigInfo.end())
        {
            m_IP = cofigItem->second;
        }
        cofigItem = m_mapConfigInfo.find("port");
        if (cofigItem != m_mapConfigInfo.end())
        {
            m_port = std::stoi(cofigItem->second);
        }
        cofigItem = m_mapConfigInfo.find("max-client-number");
        if (cofigItem != m_mapConfigInfo.end())
        {
            m_maxClientNumber = std::stoi(cofigItem->second);
        }
        cofigItem = m_mapConfigInfo.find("accept-client-batch-number");
        if (cofigItem != m_mapConfigInfo.end())
        {
            m_acceptClientBatchNumber = std::stoi(cofigItem->second);
        }
        cofigItem = m_mapConfigInfo.find("clear-client-batch-number");
        if (cofigItem != m_mapConfigInfo.end())
        {
            m_clearClientBatchNumber = std::stoi(cofigItem->second);
        }
        cofigItem = m_mapConfigInfo.find("event-poll-wait-time");
        if (cofigItem != m_mapConfigInfo.end())
        {
            m_eventPollWaitTime = std::stoi(cofigItem->second);
        }
        cofigItem = m_mapConfigInfo.find("clear-client-interval");
        if (cofigItem != m_mapConfigInfo.end())
        {
            m_clearClientInterval = std::stoi(cofigItem->second);
        }
        cofigItem = m_mapConfigInfo.find("order-service-thread-number");
        if (cofigItem != m_mapConfigInfo.end())
        {
            m_threadNum = std::stoi(cofigItem->second);
        }
        cout << "IP: " << m_IP << endl;
        cout << "m_port: " << m_port << endl;
        cout << "m_maxClientNumber: " << m_maxClientNumber << endl;
        cout << "m_acceptClientBatchNumber: " << m_acceptClientBatchNumber << endl;
        cout << "m_clearClientBatchNumber: " << m_clearClientBatchNumber << endl;
        cout << "m_eventPollWaitTime: " << m_eventPollWaitTime << endl;
        cout << "m_clearClientInterval: " << m_clearClientInterval << endl;
        cout << "m_threadNum: " << m_threadNum << endl;
    }
    else
    {
        cout << "Cannot open config file setting.ini, path: " << m_filePath;
    }
}