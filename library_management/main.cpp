#include "mainwindow.h"
#include "dbmanager.h"
#include "branchdialog.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  try {
    db::DBManager db;
    MainWindow w(&db);
    BranchDialog branch_dialog(&db, w.window());
    w.show();
    branch_dialog.show();
    a.exec();
  } catch (const std::runtime_error& e_arg) {
    QMessageBox::information(a.desktop()->screen(), "info", e_arg.what());
  }

  return 0;
}
