#include "dbmanager.h"
#include <QDebug>
#include <QSqlError>
#include <exception>

namespace db {

DBManager::DBManager(QString& user_name, QString& password)
  : mysql_db_(QSqlDatabase::addDatabase("QMYSQL")),
    branch_id_(1) {
  mysql_db_.setHostName("localhost");
  mysql_db_.setDatabaseName("Library");
  mysql_db_.setUserName(user_name);
  mysql_db_.setPassword(password);
  if (!mysql_db_.open()) {
    std::string err_msg("[db] Open database failed: ");
    err_msg += mysql_db_.lastError().text().toStdString();
    throw std::runtime_error(err_msg);
  } else {
    db_query_ = new QSqlQuery(mysql_db_);
    qDebug() << "[db] Database Connected!";
  }
}

DBManager::~DBManager(void) {
  delete db_query_;
  mysql_db_.close();
}

QSqlQuery DBManager::Query(QString& query_str) {
  return QSqlQuery(query_str, mysql_db_);
}

QSqlQuery* DBManager::GetQuery(void) {
  return db_query_;
}

void DBManager::SetBranch(const QString& name, const int& id) {
  branch_name_ = name;
  branch_id_ = id;
  qDebug() << "Branch updated to " << branch_name_;
}

} // namespace db
