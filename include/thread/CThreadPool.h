#ifndef CTHREAD_POOL_H
#define CTHREAD_POOL_H
#include "CThreadTask.h"
#include <map>
#include <string>
using namespace std;

typedef struct TaskStatus {
    string id;
    int status;
} TaskStatus;

class CThreadPool {
private:
    static vector<CThreadTask*> m_vecTaskList;    //任务列表
    
    static bool shutdown;   //线程退出标志
    int m_threadNum;   //线程池中启动的线程数
    pthread_t *pthread_id;
  
    static pthread_mutex_t m_pthreadMutex;  //线程同步锁
    static pthread_cond_t m_pthreadCond;    //线程同步条件变量
  
protected:
    static void* ThreadFunc(void *threadData);  //新线程的线程回调函数
    bool Create();   //创建线程池中的线程
  
public:

    CThreadPool(int threadNum);
    bool AddTask(CThreadTask *task);   //把任务添加到任务队列中
    bool Start();
    bool Stop();  //使线程池中的所有线程退出
    bool Quit();
    int getTaskSize();  //获取当前任务队列中的任务数
    static map<string, TaskStatus *> m_mapTaskStatus;    //任务列表
    static pthread_mutex_t m_mapTaskStatusMutex;  //线程同步锁
};
#endif