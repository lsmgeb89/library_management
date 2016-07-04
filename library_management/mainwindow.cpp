#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include <QVariant>

MainWindow::MainWindow(db::DBManager* db, QWidget *parent)
  : QMainWindow(parent),
    db_(db),
    ui_(new Ui::MainWindow),
    search_model_(new QSqlQueryModel(this)),
    loans_model_(new QSqlQueryModel(this)),
    borrower_model_(new QSqlQueryModel(this)) {
  ui_->setupUi(this);
  ui_->resultView->setModel(search_model_);
  ui_->result_view->setModel(loans_model_);
  ui_->borrower_table_view->setModel(borrower_model_);
}

MainWindow::~MainWindow() {
  delete ui_;
}

void MainWindow::on_searchButton_clicked() {
  QStringList search_words_list = ui_->inputEdit->text().split(QRegExp("\\s"));

  QString query_str =
    "SELECT B.Isbn AS 'ISBN(10)', B.Title AS 'Book Title', A.Name AS 'Author Name(s)', C.No_of_Copies AS 'Number of Copies' "
    "FROM   BOOK AS B, AUTHORS AS A, BOOK_AUTHORS AS U, BOOK_COPIES AS C "
    "WHERE  (B.Isbn = U.Isbn AND B.Isbn = C.Isbn AND U.Author_id = A.Author_id)";

  query_str += " AND C.Branch_id = '";
  query_str += QString::number(db_->GetBranchId());
  query_str += "' AND (";
  for (int i = 0; i < search_words_list.size(); i++) {
    if (i != 0) {
      query_str += " OR ";
    }
    query_str += "B.Title LIKE '%";
    query_str += search_words_list[i];
    query_str += "%' OR ";
    query_str += "A.Name LIKE '%";
    query_str += search_words_list[i];
    query_str += "%' OR ";
    query_str += "B.Isbn LIKE '%";
    query_str += search_words_list[i];
    query_str += "%'";
  }
  query_str += ");";

  QSqlQuery* query = db_->GetQuery();
  query->clear();
  query->prepare(query_str);
  if (query->exec()) {
    qDebug() << query->lastError().text();
  }
  search_model_->setQuery(*query);
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
  ui_->result_view->show();
}

void MainWindow::on_check_in_button_clicked() {
  QString query_book_loan_str;
  if (ui_->check_in_button->text() == "Search") {
    loans_model_->clear();
    ui_->result_view->show();
    query_book_loan_str =
      "SELECT * \
       FROM   BOOK_LOANS \
       WHERE  Book_id = " + ui_->book_edit->text() + " AND Card_no = '" + ui_->borrower_edit->text() + "';";
    QSqlQuery sql_query = db_->Query(query_book_loan_str);
    if (!sql_query.exec()) {
      qDebug() << "query error: ";
    }
    if (sql_query.size() != 1) {
      qDebug() << "We cannot uniquely locate one record!";
      return;
    }
    loans_model_->setQuery(sql_query);
    ui_->result_view->show();


    sql_query.clear();
    ui_->check_in_button->setText("Check in");
  } else if (ui_->check_in_button->text() == "Check in") {
    QSqlQuery sql_query = db_->Query(query_book_loan_str);
    query_book_loan_str =
      "UPDATE BOOK_LOANS \
       SET    Date_in = :date_in \
       WHERE  Book_id = :book_id AND Card_no = :card_no";
    sql_query.prepare(query_book_loan_str);
    sql_query.bindValue(":date_in", QDate::currentDate());
    sql_query.bindValue(":book_id", ui_->book_edit->text());
    sql_query.bindValue(":card_no", ui_->borrower_edit->text());
    sql_query.exec();

    sql_query.clear();
    query_book_loan_str =
      "SELECT * \
       FROM   BOOK_LOANS \
       WHERE  Book_id = :book_id AND Card_no = :card_no";
    sql_query.prepare(query_book_loan_str);
    sql_query.bindValue(":book_id", ui_->book_edit->text());
    sql_query.bindValue(":card_no", ui_->borrower_edit->text());
    sql_query.exec();
    loans_model_->setQuery(sql_query);
    ui_->result_view->show();
    ui_->check_in_button->setText("Search");
  }
}

void MainWindow::on_create_button_clicked() {
  if (ui_->ssn_line_edit->text().isEmpty() ||
      ui_->fname_line_edit->text().isEmpty() ||
      ui_->lname_line_edit->text().isEmpty()) {
    qDebug() << "Please SSN, Name or Address!";
  }

  QSqlQuery* query = db_->GetQuery();

  QString sql_find_max_card_no =
    "SELECT MAX(CONVERT(SUBSTRING_INDEX(Card_no, 'ID', -1), UNSIGNED INTEGER)) \
     FROM   BORROWER;";
  query->prepare(sql_find_max_card_no);
  query->exec();
  query->next();
  QVariant max_card_no = query->value(0);
  qDebug() << max_card_no;
  unsigned int max_no = max_card_no.toUInt() + 1;
  QString card_no = "ID00" + QString::number(max_no);

  QString sql_insert_borrower_str =
    "INSERT INTO BORROWER (Card_no, Ssn, Fname, Lname, Address, City, State, Phone) \
     VALUES (:card_no, :ssn, :fname, :lname, :address, :city, :state, :phone)";
  query->clear();
  query->prepare(sql_insert_borrower_str);
  query->bindValue(":card_no", card_no);
  query->bindValue(":ssn", ui_->ssn_line_edit->text());
  query->bindValue(":fname", ui_->fname_line_edit->text());
  query->bindValue(":lname", ui_->lname_line_edit->text());
  query->bindValue(":address", ui_->address_line_edit->text().isEmpty() ?
    QVariant(QVariant::String) : ui_->address_line_edit->text());
  query->bindValue(":city", ui_->city_line_edit->text().isEmpty() ?
    QVariant(QVariant::String) : ui_->city_line_edit->text());
  query->bindValue(":state", ui_->state_line_edit->text().isEmpty() ?
    QVariant(QVariant::String) : ui_->state_line_edit->text());
  query->exec();

  QString sql_query_borrower =
    "SELECT * \
     FROM BORROWER \
     WHERE Card_no = :card_no";
  query->clear();
  query->prepare(sql_query_borrower);
  query->bindValue(":card_no", card_no);
  qDebug() << card_no;
  query->exec();
  borrower_model_->setQuery(*query);
  ui_->borrower_table_view->show();
}
