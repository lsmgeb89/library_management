#ifndef BRANCHDIALOG_H
#define BRANCHDIALOG_H

#include "dbmanager.h"
#include <QDialog>

namespace Ui {
  class BranchDialog;
}

class BranchDialog : public QDialog {
  Q_OBJECT

public:
  explicit BranchDialog(db::DBManager *db = nullptr,
                        QWidget *parent = nullptr);
  ~BranchDialog();

private slots:
  void on_pushButton_clicked();

private:
  Ui::BranchDialog* ui_;
  db::DBManager* db_;
};

#endif // BRANCHDIALOG_H
