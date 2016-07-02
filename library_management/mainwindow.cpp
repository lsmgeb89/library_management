#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(db::DBManager* db, QWidget *parent)
  : QMainWindow(parent),
    ui_(new Ui::MainWindow),
    search_model_(new QSqlQueryModel(this)),
    db_(db) {
  ui_->setupUi(this);
}

MainWindow::~MainWindow() {
  delete ui_;
}

void MainWindow::on_searchButton_clicked() {
  QString query_str =
    "SELECT B.Isbn, B.Title, A.Name, C.No_of_Copies \
     FROM   BOOK AS B, AUTHORS AS A, BOOK_AUTHORS AS U, BOOK_COPIES AS C \
     WHERE  B.Title = '" + ui_->inputEdit->text() +
    "' AND C.Branch_id = '1' AND B.Isbn = U.Isbn AND B.Isbn = C.Isbn AND U.Author_id = A.Author_id;";
  search_model_->setQuery(db_->Query(query_str));
  ui_->resultView->setModel(search_model_);
  ui_->resultView->show();
}
