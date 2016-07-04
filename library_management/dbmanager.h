#ifndef DBMANAGER_H_
#define DBMANAGER_H_

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace db {

class DBManager {
public:
  DBManager(void);

  ~DBManager(void);

  QSqlQuery Query(QString& query_str);

  QSqlQuery* GetQuery(void);

  void SetBranch(const QString& name, const int& id);

  int GetBranchId(void) { return branch_id_; }

private:
  QSqlDatabase mysql_db_;
  QSqlQuery* db_query_;

  QString branch_name_;
  int branch_id_;
};

} // namespace db

#endif // DBMANAGER_H_
