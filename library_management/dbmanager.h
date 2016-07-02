#ifndef DBMANAGER_H_
#define DBMANAGER_H_

#include <QSqlDatabase>
#include <QSqlQuery>

namespace db {

class DBManager {
public:
  DBManager(void);

  ~DBManager(void);

  QSqlQuery Query(QString& query_str);

private:
  QSqlDatabase mysql_db_;
};

} // namespace db

#endif // DBMANAGER_H_
