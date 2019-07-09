#ifndef CMYSQL_DB_H
#define CMYSQL_DB_H
#include <string>
#include <iostream>
#include <mysql/mysql.h>
#include <stack>
#include <algorithm>
#include <vector>

using namespace std;

class CDiskDB{
  public:
    CDiskDB(int id, string ip, int port, string user, string pwd, string dbname);
    ~CDiskDB();
    bool ExecuteQuery(string sql);
    bool ExecuteUpdate(string sql);
    bool ExecuteInsert(string sql);
    bool ExecuteDelete(string sql);
    void Close();
    bool TransationStart();
    bool TransationCommit();
    bool TransationRollback();
    bool AutoCommit(int value);
    bool Commit();
    bool Featch();
    bool GetField(int column, string& field);
    bool FreeResult();
    MYSQL_RES* m_result;
    const int m_id;
  private:
    
    MYSQL *m_mysql;
    MYSQL_ROW m_row;
    
    MYSQL_FIELD* m_field;
    string m_IP;
    int m_port;
    string m_user;
    string m_password;
    string m_dbName;
};
#endif