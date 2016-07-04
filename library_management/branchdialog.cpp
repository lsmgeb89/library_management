#include "branchdialog.h"
#include "ui_branchdialog.h"

#include <QDebug>

BranchDialog::BranchDialog(db::DBManager *db, QWidget *parent)
  : QDialog(parent),
    ui_(new Ui::BranchDialog),
    db_(db) {
  ui_->setupUi(this);
  QSqlQuery* query = db_->GetQuery();
  query->clear();
  query->exec("SELECT Branch_name FROM LIBRARY_BRANCH;");
  for (int i = 0; i < query->size(); i++) {
    query->next();
    ui_->comboBox->addItem(query->value(0).toString());
  }
}

BranchDialog::~BranchDialog() {
  delete ui_;
}

void BranchDialog::on_pushButton_clicked() {
  QSqlQuery* query = db_->GetQuery();
  query->clear();
  query->prepare("SELECT Branch_id FROM LIBRARY_BRANCH WHERE Branch_name = :name");
  query->bindValue(":name", ui_->comboBox->currentText());
  query->exec();
  query->next();
  db_->SetBranch(ui_->comboBox->currentText(), query->value(0).toInt());
  this->close();
}
