#ifndef SHARE_MUTEX_H
#define SHARE_MUTEX_H

#define SHARE_MUTEX_FLIE_LOCK 0
#include <sys/file.h>
#include <string.h>
#include <string>
#include <unistd.h>

using namespace std;

class CShareMutex
{
private:
    int m_type;
    int m_fd;
    string m_fileName;

public:
    CShareMutex(string fileName);
    ~CShareMutex();
    bool create();
    bool destory();
    bool tryLock();
    bool lock();
    bool unLock();
};
#endif