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

private:
  Ui::MainWindow *ui_;
  QSqlQueryModel* search_model_;
  db::DBManager *db_;
};

#endif // MAINWINDOW_H
