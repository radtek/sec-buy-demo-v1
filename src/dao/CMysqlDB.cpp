#include "../../include/dao/CDiskDB.h"
#include <exception>
#include "../../include/log/log.h"

CDiskDB::CDiskDB(int id, string ip, int port, string username, string pwd, string dbname)
    : m_id(id), m_IP(ip), m_port(port), m_user(username), m_password(pwd), m_dbName(dbname)
{
    LOG_DEBUG("In CDiskDB::CDiskDB id:%d, IP:%s, port:%d, username:%s, password:%s, dbname:%s",
              id, ip.c_str(), port, username.c_str(), pwd.c_str(), dbname.c_str());
    do
    {
        m_mysql = mysql_init(NULL);
        if (m_mysql == NULL)
        {
            LOG_ERROR("CDiskDB::CDiskDB mysql_init fail id:%d, error:%s", id, mysql_error(m_mysql));
            break;
        }
        if (m_mysql && !mysql_real_connect(m_mysql, m_IP.c_str(), username.c_str(),
                                           m_password.c_str(), m_dbName.c_str(), m_port, NULL, 0))
        {
            LOG_ERROR("CDiskDB::CDiskDB mysql_real_connect fail id:%d, error:%s", id, mysql_error(m_mysql));
            break;
        }
        int ping = mysql_ping(m_mysql);
        LOG_DEBUG("Out CDiskDB::CDiskDB id:%d mysql_ping:%d", id, ping);
    } while (false);

    LOG_DEBUG("Out CDiskDB::CDiskDB id:%d", id);
}

CDiskDB::~CDiskDB()
{
    LOG_DEBUG("In CDiskDB::~CDiskDB id:%d", m_id);
    if (!m_mysql)
    {
        mysql_close(m_mysql);
    }
    LOG_DEBUG("Out CDiskDB::~CDiskDB id:%d", m_id);
}

bool CDiskDB::ExecuteQuery(string sql)
{
    LOG_DEBUG("In CDiskDB::ExecuteQuery id:%d, sql:%s", m_id, sql.c_str());
    bool result = true;
    try
    {
        do
        {
            if (mysql_query(m_mysql, sql.c_str()))
            {
                LOG_WARN("CDiskDB::ExecuteQuery mysql_query fail error:%s", mysql_error(m_mysql));
                result = false;
                break;
            }

            m_result = mysql_store_result(m_mysql);
            if (m_result == nullptr)
            {
                LOG_WARN("CDiskDB::ExecuteQuery mysql_store_result fail error:%s", mysql_error(m_mysql));
                result = false;
                break;
            }

        } while (false);
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::ExecuteQuery catch");
        LOG_WARN("CDiskDB::ExecuteQuery exception id:%d, error:%s", m_id, e.what());
        if (m_result)
        {
            mysql_free_result(m_result);
            m_result = nullptr;
        }
        result = false;
    }

    LOG_DEBUG("Out CDiskDB::ExecuteQuery id:%d, result:%d", m_id, result);
    return result;
}

bool CDiskDB::Featch()
{
    LOG_DEBUG("In CDiskDB::Featch id:%d", m_id);
    bool result = true;
    try
    {
        do
        {
            if (m_result == nullptr)
            {
                LOG_WARN("CDiskDB::Featch result == nullptr id:%d", m_id);
                result = false;
                break;
            }

            if (!(m_row = mysql_fetch_row(m_result)))
            {
                LOG_WARN("CDiskDB::Featch mysql_fetch_row fail id:%d", m_id);
                result = false;
                break;
            }
        } while (false);
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::Featch exception:%s", e.what());
        result = false;
    }

    LOG_DEBUG("Out CDiskDB::Featch id:%d", m_id);
    return result;
}

bool CDiskDB::GetField(int column, string &field)
{
    try
    {
        if (!m_row)
        {
            LOG_WARN("CDiskDB::GetField m_row == null id:%d", m_id);
            return false;
        }
        field = m_row[column];
        return true;
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::GetField catch");
        LOG_WARN("CDiskDB::GetField exception:%s", e.what());
        return false;
    }
}

bool CDiskDB::FreeResult()
{
    LOG_DEBUG("In CDiskDB::FreeResult");
    bool result = true;
    try
    {
        if (!m_result)
        {
            LOG_DEBUG("CDiskDB::FreeResult no need free id:%d", m_id);
            return true;
        }
        mysql_free_result(m_result);
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::FreeResult catch");
        LOG_WARN("CDiskDB::FreeResult exception:%s", e.what());
        result = false;
    }
    LOG_DEBUG("Out CDiskDB::FreeResult result:%d", result);
    return result;
}

bool CDiskDB::ExecuteUpdate(string sql)
{
    LOG_DEBUG("In CDiskDB::ExecuteUpdate id:%d", m_id);
    LOG_DEBUG("CDiskDB::ExecuteUpdate sql:%s", sql.c_str());
    bool result = true;
    try
    {
        if (mysql_query(m_mysql, sql.c_str()))
        {
            LOG_WARN("CDiskDB::ExecuteUpdate mysql_query fail");
            result = false;
        }
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::ExecuteUpdate catch");
        LOG_WARN("CDiskDB::ExecuteUpdate exception:%s", e.what());
        result = false;
    }
    LOG_DEBUG("Out CDiskDB::ExecuteUpdate result:%d", result);
    return result;
}

bool CDiskDB::ExecuteInsert(string sql)
{
    LOG_DEBUG("In CDiskDB::ExecuteInsert id:%d", m_id);
    LOG_DEBUG("CDiskDB::ExecuteInsert sql:%s", sql.c_str());
    bool result = true;
    try
    {
        if (mysql_query(m_mysql, sql.c_str()))
        {
            LOG_WARN("CDiskDB::ExecuteInsert mysql_query fail");
            result = false;
        }
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::ExecuteInsert catch");
        LOG_WARN("CDiskDB::ExecuteInsert exception:%s", e.what());
        result = false;
    }
    LOG_DEBUG("Out CDiskDB::ExecuteInsert result:%d", result);
    return result;
}

bool CDiskDB::ExecuteDelete(string sql)
{
    LOG_DEBUG("In CDiskDB::ExecuteDelete id:%d", m_id);
    LOG_DEBUG("CDiskDB::ExecuteDelete sql:%s", sql.c_str());
    bool result = true;
    try
    {
        if (mysql_query(m_mysql, sql.c_str()))
        {
            LOG_WARN("CDiskDB::ExecuteDelete mysql_query fail");
            result = false;
        }
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::ExecuteDelete catch");
        LOG_WARN("CDiskDB::ExecuteDelete exception:%s", e.what());
        result = false;
    }
    LOG_DEBUG("Out CDiskDB::ExecuteDelete result:%d", result);
    return result;
}

void CDiskDB::Close()
{
    LOG_DEBUG("In CDiskDB::Close id:%d", m_id);
    bool result = true;
    try
    {
        mysql_close(m_mysql);
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::Close catch");
        LOG_WARN("CDiskDB::Close exception:%s", e.what());
        result = false;
    }
    LOG_DEBUG("Out CDiskDB::Close result:%d", result);

}

bool CDiskDB::AutoCommit(int mode)
{
    LOG_DEBUG("In CDiskDB::AutoCommit id:%d, mode:%d", m_id, mode);
    bool result = true;
    try
    {
        result = mysql_autocommit(m_mysql, mode);
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::AutoCommit catch");
        LOG_WARN("CDiskDB::AutoCommit exception:%s", e.what());
        result = false;
    }
    LOG_DEBUG("Out CDiskDB::AutoCommit result:%d", result);
    return result;
}

bool CDiskDB::TransationStart()
{
    LOG_DEBUG("In CDiskDB::TransationStart id:%d", m_id);
    bool result = true;
    try
    {
        if(mysql_commit(m_mysql)){
            result = false;
        }
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::TransationStart catch");
        LOG_WARN("CDiskDB::TransationStart exception:%s", e.what());
        result = false;
    }
    LOG_DEBUG("Out CDiskDB::TransationStart result:%d", result);
    return result;
}

bool CDiskDB::TransationCommit()
{
    LOG_DEBUG("In CDiskDB::TransationCommit id:%d", m_id);
    bool result = true;
    try
    {
        if(mysql_commit(m_mysql)){
            result = false;
        }
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::TransationCommit catch");
        LOG_WARN("CDiskDB::TransationCommit exception:%s", e.what());
        result = false;
    }
    LOG_DEBUG("Out CDiskDB::TransationCommit result:%d", result);
    return result;
}

bool CDiskDB::TransationRollback()
{
    LOG_DEBUG("In CDiskDB::TransationRollback id:%d", m_id);
    bool result = true;
    try
    {
        result = mysql_rollback(m_mysql);
    }
    catch (exception &e)
    {
        LOG_WARN("CDiskDB::TransationRollback catch");
        LOG_WARN("CDiskDB::TransationRollback exception:%s", e.what());
        result = false;
    }
    LOG_DEBUG("Out CDiskDB::TransationRollback result:%d", result);
    return result;
}