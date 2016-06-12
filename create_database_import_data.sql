DROP DATABASE IF EXISTS Library;
CREATE DATABASE Library;

USE Library;

CREATE TABLE BOOK (
  Isbn        CHAR(13)    NOT NULL    DEFAULT '0000000000000',
  Title       VARCHAR(64) NOT NULL,
  CONSTRAINT  pk_book     PRIMARY KEY (Isbn)
);

CREATE TABLE AUTHORS (
  Author_id   CHAR(9)     NOT NULL    DEFAULT '000000000',
  Name        VARCHAR(64) NOT NULL,
  CONSTRAINT  pk_authors  PRIMARY KEY (Author_id)
);

CREATE TABLE BOOK_AUTHORS (
  Author_id   CHAR(9)                   NOT NULL    DEFAULT '000000000',
  Isbn        CHAR(13)                  NOT NULL    DEFAULT '0000000000000',
  CONSTRAINT  pk_book_authors           PRIMARY KEY (Author_id, Isbn),
  CONSTRAINT  kf_book_authors_author_id FOREIGN KEY (Author_id) REFERENCES AUTHORS(Author_id)
              ON UPDATE CASCADE,
  CONSTRAINT  fk_book_authors_isbn      FOREIGN KEY (Isbn)      REFERENCES BOOK(Isbn)
              ON UPDATE CASCADE
);

CREATE TABLE LIBRARY_BRANCH (
  Branch_id   VARCHAR(32)       NOT NULL,
  Branch_name VARCHAR(32)       NOT NULL,
  Address     VARCHAR(64)       NOT NULL,
  CONSTRAINT  pk_library_branch PRIMARY KEY (Branch_id)
);

CREATE TABLE BOOK_COPIES (
  Book_id     CHAR(10)                  NOT NULL,
  Isbn        CHAR(13)                  NOT NULL,
  Branch_id   VARCHAR(32)               NOT NULL,
  CONSTRAINT  pk_book_copies            PRIMARY KEY (Book_id),
  CONSTRAINT  pk_book_copies_isbn       FOREIGN KEY (Isbn)      REFERENCES BOOK(Isbn)
              ON UPDATE CASCADE,
  CONSTRAINT  pk_book_copies_branch_id  FOREIGN KEY (Branch_id) REFERENCES LIBRARY_BRANCH(Branch_id)
              ON UPDATE CASCADE
);

CREATE TABLE BORROWER (
  Card_no     CHAR(8)     NOT NULL    DEFAULT '00000000',
  Ssn         CHAR(10)    NOT NULL,
  Fname       VARCHAR(16) NOT NULL,
  Lname       VARCHAR(16) NOT NULL,
  Address     VARCHAR(64) NOT NULL,
  Phone       CHAR(10)    NOT NULL,
  CONSTRAINT  pk_borrower PRIMARY KEY (Card_no)
);

CREATE TABLE BOOK_LOANS (
  Loan_id     CHAR(10)              NOT NULL,
  Book_id     CHAR(10)              NOT NULL,
  Card_no     CHAR(8)               NOT NULL,
  Date_out    DATE,
  Due_date    DATE,
  Date_in     DATE,
  CONSTRAINT  pk_book_loans         PRIMARY KEY (Loan_id),
  CONSTRAINT  fk_book_loans_book_id FOREIGN KEY (Book_id) REFERENCES BOOK_COPIES(Book_id)
              ON UPDATE CASCADE,
  CONSTRAINT  fk_book_loans_card_no FOREIGN KEY (Card_no) REFERENCES BORROWER(Card_no)
              ON UPDATE CASCADE
);

CREATE TABLE FINES (
  Loan_id     CHAR(10)              NOT NULL,
  Fine_amt    DECIMAL(10,2)         NOT NULL,
  Paid        BOOLEAN               NOT NULL,
  CONSTRAINT  pk_fines              PRIMARY KEY (Loan_id),
  CONSTRAINT  fk_fines_load_id      FOREIGN KEY (Loan_id) REFERENCES BOOK_LOANS(Loan_id)
              ON UPDATE CASCADE
);
