#ifndef CTHREAD_TASK_H
#define CTHREAD_TASK_H
#include <vector>
#include <string>
#include <pthread.h>
#include "../messagequeue/CMsgProducer.h"

using namespace std;

/*执行任务的类：设置任务数据并执行*/
class CThreadTask {
protected:
    string m_strTaskName;   //任务的名称
    void* m_ptrData;    //要执行的任务的具体数据

public:
    CThreadTask() = default;
    CThreadTask(string taskName, string id): m_strTaskName(taskName), m_ptrData(NULL),m_id(id) {}
    virtual bool Run(CMsgProducer* producer) = 0;
    void setData(void* data);   //设置任务数据
    virtual ~CThreadTask() {};
    string m_id;   //任务的名称
};

#endif
