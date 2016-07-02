#include "mainwindow.h"
#include "dbmanager.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  try {
    db::DBManager db;
    MainWindow w(&db);
    w.show();
    a.exec();
  } catch (const std::runtime_error& e_arg) {
    QMessageBox::information(a.desktop()->screen(), "info", e_arg.what());
  }

  return 0;
}
