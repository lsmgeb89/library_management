#include "mainwindow.h"
#include "dbmanager.h"
#include "branchdialog.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QString user_name;
  QString password;

  if (argc != 3) {
    QMessageBox::critical(a.desktop()->screen(),
                          "Error",
                          "Wrong Parameter!<br>Parameter should be MySQL username and password!");
    return 0;
  } else {
    user_name = argv[1];
    password = argv[2];
  }

  try {
    db::DBManager db(user_name, password);
    MainWindow w(&db);
    w.show();
    a.exec();
  } catch (const std::runtime_error& e_arg) {
    QMessageBox::critical(a.desktop()->screen(), "Error", e_arg.what());
  }

  return 0;
}
