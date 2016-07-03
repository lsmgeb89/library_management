#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include <QVariant>

MainWindow::MainWindow(db::DBManager* db, QWidget *parent)
  : QMainWindow(parent),
    db_(db),
    ui_(new Ui::MainWindow),
    search_model_(new QSqlQueryModel(this)),
    loans_model_(new QSqlQueryModel(this)) {
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

void MainWindow::on_check_out_button_clicked() {
  QString query_book_id_str =
    "SELECT C.Book_id \
     FROM   BOOK AS B, BOOK_COPIES AS C \
     WHERE  B.Title = '" + ui_->book_edit->text() +
    "' AND C.Branch_id = '1' AND B.Isbn = C.Isbn;";
  QSqlQuery sql_query = db_->Query(query_book_id_str);
  if (!sql_query.exec()) {
    qDebug() << "query error: ";
  }
  sql_query.next();
  QVariant book_id = sql_query.value(0);

  QSqlQuery& sql_insert(sql_query);
  QString insert_book_loans_str =
    "INSERT INTO BOOK_LOANS (Book_id, Card_no, Date_out, Due_date, Date_in) \
     VALUES      (:book_id, :card_no, :date_out, :due_date, :date_in)";
  sql_insert.prepare(insert_book_loans_str);
  sql_insert.bindValue(":book_id", book_id);
  sql_insert.bindValue(":card_no", ui_->borrower_edit->text());
  QDate date = QDate::currentDate();
  sql_insert.bindValue(":date_out", date);
  sql_insert.bindValue(":due_date", date.addDays(14));
  sql_insert.bindValue(":date_in", QVariant(QVariant::Date));
  sql_insert.exec();

  QString query_result_str =
    "SELECT * \
     FROM   BOOK_LOANS \
     WHERE  Book_id = " + book_id.toString() + " AND Card_no = '" + ui_->borrower_edit->text() + "';";
  loans_model_->setQuery(db_->Query(query_result_str));
  ui_->result_view->setModel(loans_model_);
  ui_->result_view->show();
}
