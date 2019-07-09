#include "../../include/ipc/sharemutex.h"
#include "../../include/log/log.h"

CShareMutex::CShareMutex(string fileName) : m_fileName(fileName)
{
    LOG_DEBUG("In CShareMutex CShareMutex filename:%s", m_fileName.c_str());
    create();
    LOG_DEBUG("Out CShareMutex CShareMutex");
}

CShareMutex::~CShareMutex()
{
    LOG_DEBUG("In CShareMutex ~CShareMutex filename:%s", m_fileName.c_str());
    destory();
    LOG_DEBUG("Out CShareMutex ~CShareMutex");
}

bool CShareMutex::create()
{
    LOG_DEBUG("In CShareMutex create filename:%s", m_fileName.c_str());
    bool result = true;
    m_fd = open(m_fileName.c_str(), O_RDWR | O_CREAT, 0644);
    if (m_fd == -1)
    {
        result = false;
        LOG_WARN("CShareMutex create open fail filename:%s", m_fileName.c_str());
        return result;
    }
    if (unlink(m_fileName.c_str()) == -1)
    {
        LOG_WARN("CShareMutex create unlink fail filename:%s", m_fileName.c_str());
    }
    LOG_DEBUG("Out CShareMutex create filename:%s, result:%d", m_fileName.c_str(), result);
    return result;
}

bool CShareMutex::destory()
{
    LOG_DEBUG("In CShareMutex destory filename:%s", m_fileName.c_str());
    close(m_fd);
    LOG_DEBUG("Out CShareMutex destory filename:%s", m_fileName.c_str());
}

bool CShareMutex::tryLock()
{
    LOG_DEBUG("In CShareMutex tryLock filename:%s", m_fileName.c_str());
    struct flock fl;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = 0;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;

    bool result = true;
    if (fcntl(m_fd, F_SETLK, &fl) == -1)
    {
        result = false;
    }
    LOG_DEBUG("Out CShareMutex tryLock filename:%s", m_fileName.c_str());
    return result;
}

bool CShareMutex::lock()
{
    LOG_DEBUG("In CShareMutex lock filename:%s", m_fileName.c_str());
    struct flock fl;

    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = 0;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;

    bool result = true;
    if (fcntl(m_fd, F_SETLKW, &fl) == -1)
    {
        result = false;
    }
    LOG_DEBUG("Out CShareMutex lock filename:%s", m_fileName.c_str());
    return result;
}

bool CShareMutex::unLock()
{
    LOG_DEBUG("In CShareMutex unLock filename:%s", m_fileName.c_str());
    struct flock fl;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = 0;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;

    bool result = true;
    if (fcntl(m_fd, F_SETLK, &fl) == -1)
    {
        result = false;
    }
    LOG_DEBUG("Out CShareMutex unLock filename:%s", m_fileName.c_str());
    return result;
}