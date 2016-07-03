#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbmanager.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQueryModel>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(db::DBManager *db = nullptr,
                      QWidget* parent = nullptr);
  ~MainWindow();

private slots:
  void on_searchButton_clicked();

  void on_check_out_button_clicked();

  void on_check_in_button_clicked();

  void on_create_button_clicked();

private:
  db::DBManager *db_;
  Ui::MainWindow *ui_;
  QSqlQueryModel* search_model_;
  QSqlQueryModel* loans_model_;
  QSqlQueryModel* borrower_model_;
};

#endif // MAINWINDOW_H
