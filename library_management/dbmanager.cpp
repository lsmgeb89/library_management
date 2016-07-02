#include "dbmanager.h"
#include <QDebug>
#include <QSqlError>
#include <exception>

namespace db {

DBManager::DBManager(void)
  : mysql_db_(QSqlDatabase::addDatabase("QMYSQL")) {
  mysql_db_.setHostName("localhost");
  mysql_db_.setDatabaseName("library");
  mysql_db_.setUserName("root");
  mysql_db_.setPassword("890811");
  if (!mysql_db_.open()) {
    std::string err_msg("[db] Open database failed: ");
    err_msg += mysql_db_.lastError().text().toStdString();
    throw std::runtime_error(err_msg);
  } else {
    qDebug() << "[db] Database Connected!";
  }
}

DBManager::~DBManager(void) {
  mysql_db_.close();
}

QSqlQuery DBManager::Query(QString& query_str) {
  return QSqlQuery(query_str, mysql_db_);
}

} // namespace db
