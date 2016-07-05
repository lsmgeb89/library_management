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
  QString book_id = ui_->book_edit->text();
  QString card_no = ui_->borrower_edit->text();
  int branch_id = db_->GetBranchId();

  // Check whether the book is available or not
  QString query_available =
    "SELECT No_of_Copies "
    "FROM   BOOK_COPIES "
    "WHERE  Book_id = :book_id AND Branch_id = :branch_id";
  QSqlQuery* query = db_->GetQuery();
  query->clear();
  query->prepare(query_available);
  query->bindValue(":book_id", book_id);
  query->bindValue(":branch_id", branch_id);
  if (!query->exec()) {
    qDebug() << query->lastError().text();
  }
  query->next();
  if (query->value(0).toInt() < 1) {
    QMessageBox::information(this, "Error", "Check out failed! <br> The book is not available now!");
    return;
  }

  // Check whether this borrower has three active loans
  QString& query_loans(query_available);
  query_loans =
    "SELECT Count(*) "
    "FROM   BOOK_LOANS "
    "WHERE  (Card_no = :card_no) AND (Date_in IS NULL)";
  query->clear();
  query->prepare(query_loans);
  query->bindValue(":card_no", card_no);
  if (!query->exec()) {
    qDebug() << query->lastError().text();
  }
  query->next();
  if (query->value(0).toInt() == 3) {
    QMessageBox::information(this, "Error", "Check out failed! <br> The Borrower has 3 active book loans!");
    return;
  }

  // Check overdue
  QString& query_overdue(query_available);
  query_overdue =
    "SELECT *"
    "FROM   BOOK_LOANS "
    "WHERE  (Card_no = :card_no) AND (Due_date < :current_date)";
  query->clear();
  query->prepare(query_overdue);
  query->bindValue(":card_no", card_no);
  query->bindValue(":current_date", QDate::currentDate());
  query->exec();
  if (query->size()) {
    QMessageBox::information(this, "Error", "Check out failed! <br> The Borrower has " +
                             QString::number(query->size()) + " overdue book loans!");
    loans_model_->setQuery(*query);
    ui_->result_view->show();
    return;
  } else {
    qDebug() << "The borrower has no overdue book loans.";
  }

  // Check unpaid
  QString& query_unpaid(query_available);
  query_unpaid =
    "SELECT * "
    "FROM   BOOK_LOANS NATURAL JOIN FINES "
    "WHERE  Card_no = :card_no AND Paid = FALSE";
  query->clear();
  query->prepare(query_unpaid);
  query->bindValue(":card_no", card_no);
  if (!query->exec()) {
    qDebug() << query->lastError().text();
  }
  if (query->size()) {
    QMessageBox::information(this, "Error", "Check out failed! <br> The Borrower has " +
                             QString::number(query->size()) + " unpaid fines!");
    loans_model_->setQuery(*query);
    ui_->result_view->show();
    return;
  }

  // Insert a record
  QString& insert_book_loans_str(query_available);
  insert_book_loans_str =
    "INSERT INTO BOOK_LOANS (Book_id, Card_no, Date_out, Due_date, Date_in) "
    "VALUES      (:book_id, :card_no, :date_out, :due_date, :date_in)";
  query->clear();
  query->prepare(insert_book_loans_str);
  query->bindValue(":book_id", book_id);
  query->bindValue(":card_no", card_no);
  QDate date = QDate::currentDate();
  query->bindValue(":date_out", date);
  query->bindValue(":due_date", date.addDays(14));
  query->bindValue(":date_in", QVariant(QVariant::Date));
  if (!query->exec()) {
    qDebug() << query->lastError().text();
  } else {
    QMessageBox::information(this, "Success", "Check out success!");
  }

  // Update number of copies
  QString& update_copies_str(query_available);
  update_copies_str =
    "UPDATE BOOK_COPIES "
    "SET    No_of_Copies = No_of_Copies - 1 "
    "WHERE  Book_id = :book_id AND Branch_id = :branch_id";
  query->clear();
  query->prepare(update_copies_str);
  query->bindValue(":book_id", book_id);
  query->bindValue(":branch_id", branch_id);
  if (!query->exec()) {
    qDebug() << query->lastError().text();
  }

  // Show final result
  QString query_result_str =
    "SELECT * "
    "FROM   BOOK_LOANS "
    "WHERE  Book_id = :book_id AND Card_no = :card_no";
  query->clear();
  query->prepare(query_result_str);
  query->bindValue(":book_id", book_id);
  query->bindValue(":card_no", card_no);
  query->exec();
  loans_model_->setQuery(*query);
  ui_->result_view->show();
}

void MainWindow::on_check_in_button_clicked() {
  QSqlQuery* query = db_->GetQuery();
  QString book_id = ui_->book_edit->text();
  int branch_id = db_->GetBranchId();

  // Locate an active BOOK_LOANS record
  QString query_book_loan_str =
    "SELECT Loan_id, Due_date "
    "FROM   BOOK_LOANS "
    "WHERE  Book_id = :book_id";
  query->clear();
  query->prepare(query_book_loan_str);
  query->bindValue(":book_id", book_id);
  if (!query->exec()) {
    qDebug() << query->lastError().text();
  }
  if (query->size() != 1) {
    qDebug() << "We cannot uniquely locate one record!";
    return;
  }

  // Check overdue
  query->next();
  qint64 offset = query->value(1).toDate().daysTo(QDate::currentDate());
  bool overdue = offset > 0;
  float fines = 0.0f;
  if (overdue) {
    QVariant loan_id = query->value(0);
    QString& insert_fines_str(query_book_loan_str);
    insert_fines_str =
      "INSERT INTO FINES(Loan_id, Fine_amt, Paid) "
      "VALUES      (:loan_id, :fines, FALSE)";
    query->clear();
    query->prepare(insert_fines_str);
    query->bindValue(":loan_id", loan_id);
    fines = offset * 0.25f;
    query->bindValue(":fines", fines);
    query->exec();
    QMessageBox::information(this, "Warning", "You have a fine with " + QString::number(fines));
  }

  // Update a record
  QString& update_book_loan_str(query_book_loan_str);
  update_book_loan_str =
   "UPDATE BOOK_LOANS "
   "SET    Date_in = :date_in "
   "WHERE  Book_id = :book_id";
  query->clear();
  query->prepare(update_book_loan_str);
  query->bindValue(":date_in", QDate::currentDate());
  query->bindValue(":book_id", book_id);
  query->exec();

  // Update number of copies
  QString& update_copies_str(query_book_loan_str);
  update_copies_str =
    "UPDATE BOOK_COPIES "
    "SET    No_of_Copies = No_of_Copies + 1 "
    "WHERE  Book_id = :book_id AND Branch_id = :branch_id";
  query->clear();
  query->prepare(update_copies_str);
  query->bindValue(":book_id", book_id);
  query->bindValue(":branch_id", branch_id);
  if (!query->exec()) {
    qDebug() << query->lastError().text();
  }

  // Query result
  QString& query_result_str(query_book_loan_str);
  if(overdue) {
    query_result_str =
      "SELECT * "
      "FROM   BOOK_LOANS NATURAL JOIN FINES "
      "WHERE  Book_id = :book_id";
  } else {
    query_result_str =
      "SELECT * "
      "FROM   BOOK_LOANS "
      "WHERE  Book_id = :book_id";
  }
  query->clear();
  query->prepare(query_result_str);
  query->bindValue(":book_id", book_id);
  if (!query->exec()) {
    qDebug() << query->lastError().text();
  }

  // Show result
  loans_model_->setQuery(*query);
  ui_->result_view->show();
}

void MainWindow::on_create_button_clicked() {
  QSqlQuery* query = db_->GetQuery();
  QString query_str;
  QString ssn(ui_->ssn_line_edit->text());
  QString fname(ui_->fname_line_edit->text());
  QString lname(ui_->lname_line_edit->text());
  QString address(ui_->address_line_edit->text());
  QString city(ui_->city_line_edit->text());
  QString state(ui_->state_line_edit->text());
  QString phone(ui_->phone_line_edit->text());

  if (ssn.isEmpty() || fname.isEmpty() || lname.isEmpty() || address.isEmpty()) {
    QMessageBox::critical(this, "Error", "SSN, Name or Address could not be empty!");
    return;
  }

  // Check SSN
  QString& query_double_ssn(query_str);
  query_double_ssn =
    "SELECT * "
    "FROM   BORROWER "
    "WHERE  Ssn = :ssn";
  query->clear();
  query->prepare(query_double_ssn);
  query->bindValue(":ssn", ssn);
  if (!query->exec()) {
    qDebug() << query->lastError().text();
  }
  if (query->size()) {
    QMessageBox::critical(this, "Error", "Duplicate SSN Number!");
    return;
  }

  QString sql_find_max_card_no =
    "SELECT MAX(CONVERT(SUBSTRING_INDEX(Card_no, 'ID', -1), UNSIGNED INTEGER)) \
     FROM   BORROWER";
  query->clear();
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
  query->bindValue(":ssn", ssn);
  query->bindValue(":fname", fname);
  query->bindValue(":lname", lname);
  query->bindValue(":address", address.isEmpty() ? QVariant(QVariant::String) : address);
  query->bindValue(":city", city.isEmpty() ? QVariant(QVariant::String) : city);
  query->bindValue(":state", state.isEmpty() ? QVariant(QVariant::String) : state);
  query->bindValue(":phone", phone.isEmpty() ? QVariant(QVariant::String) : phone);
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

void MainWindow::on_clear_button_clicked() {
  ui_->ssn_line_edit->clear();
  ui_->fname_line_edit->clear();
  ui_->lname_line_edit->clear();
  ui_->address_line_edit->clear();
  ui_->city_line_edit->clear();
  ui_->state_line_edit->clear();
  ui_->city_line_edit->clear();
  ui_->phone_line_edit->clear();
}
