### Create Database ###
DROP DATABASE IF EXISTS Library;
CREATE DATABASE Library;
USE Library;

### Create Tables ###
CREATE TABLE BOOK (
  Isbn        CHAR(10)        NOT NULL    DEFAULT '0000000000',
  Title       VARCHAR(256)    NOT NULL,
  CONSTRAINT  pk_book         PRIMARY KEY (Isbn)
);

CREATE TABLE AUTHORS (
  Author_id   INT UNSIGNED    NOT NULL    AUTO_INCREMENT,
  Name        VARCHAR(64)     NOT NULL,
  CONSTRAINT  pk_authors      PRIMARY KEY (Author_id),
  CONSTRAINT  un_authors_name UNIQUE (Name)
);

CREATE TABLE BOOK_AUTHORS (
  Author_id   INT UNSIGNED              NOT NULL,
  Isbn        CHAR(10)                  NOT NULL    DEFAULT '0000000000',
  CONSTRAINT  pk_book_authors           PRIMARY KEY (Author_id, Isbn),
  CONSTRAINT  kf_book_authors_author_id FOREIGN KEY (Author_id) REFERENCES AUTHORS(Author_id)
              ON UPDATE CASCADE,
  CONSTRAINT  fk_book_authors_isbn      FOREIGN KEY (Isbn)      REFERENCES BOOK(Isbn)
              ON UPDATE CASCADE
);

CREATE TABLE LIBRARY_BRANCH (
  Branch_id     TINYINT(1) UNSIGNED NOT NULL,
  Branch_name   VARCHAR(32)         NOT NULL,
  Address       VARCHAR(64)         NOT NULL,
  CONSTRAINT    pk_library_branch   PRIMARY KEY (Branch_id)
);

CREATE TABLE BOOK_COPIES (
  Book_id       INT UNSIGNED              NOT NULL    AUTO_INCREMENT,
  Isbn          CHAR(10)                  NOT NULL    DEFAULT '0000000000',
  Branch_id     TINYINT(1) UNSIGNED       NOT NULL,
  No_of_Copies  INT UNSIGNED              NOT NULL,                    
  CONSTRAINT    pk_book_copies            PRIMARY KEY (Book_id),
  CONSTRAINT    pk_book_copies_isbn       FOREIGN KEY (Isbn)      REFERENCES BOOK(Isbn)
                ON UPDATE CASCADE,
  CONSTRAINT    pk_book_copies_branch_id  FOREIGN KEY (Branch_id) REFERENCES LIBRARY_BRANCH(Branch_id)
                ON UPDATE CASCADE
);

CREATE TABLE BORROWER (
  Card_no     CHAR(8)     NOT NULL    DEFAULT '00000000',
  Ssn         CHAR(11)    NOT NULL,
  Fname       VARCHAR(16) NOT NULL,
  Lname       VARCHAR(16) NOT NULL,
  Address     VARCHAR(64),
  City        VARCHAR(16),
  State       VARCHAR(16),
  Phone       CHAR(14),
  CONSTRAINT  pk_borrower PRIMARY KEY (Card_no)
);

CREATE TABLE BOOK_LOANS (
  Loan_id     INT UNSIGNED          NOT NULL    AUTO_INCREMENT,
  Book_id     INT UNSIGNED          NOT NULL,
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
  Loan_id     INT UNSIGNED           NOT NULL,
  Fine_amt    DECIMAL(10,2)          NOT NULL,
  Paid        ENUM('Paid', 'Unpaid') NOT NULL,
  CONSTRAINT  pk_fines               PRIMARY KEY (Loan_id),
  CONSTRAINT  fk_fines_loan_id       FOREIGN KEY (Loan_id) REFERENCES BOOK_LOANS(Loan_id)
              ON UPDATE CASCADE
);

### Import Data ###
LOAD DATA LOCAL INFILE 'C:/Users/ThinkPad-E531/Documents/Assignments/2016/Summer/CS 6360.0U1 - Database Design/project_1/data_source/library_branch.csv'
     INTO TABLE LIBRARY_BRANCH
   FIELDS TERMINATED BY '\t'
 ENCLOSED BY ''
    LINES TERMINATED BY '\n'
   IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE 'C:/Users/ThinkPad-E531/Documents/Assignments/2016/Summer/CS 6360.0U1 - Database Design/project_1/data_source/borrowers.csv'
     INTO TABLE BORROWER
   FIELDS TERMINATED BY ','
 ENCLOSED BY ''
    LINES TERMINATED BY '\r\n'
   IGNORE 1 ROWS
   (Card_no, Ssn, Fname, Lname, @dummy, Address, City, State, Phone);

LOAD DATA LOCAL INFILE 'C:/Users/ThinkPad-E531/Documents/Assignments/2016/Summer/CS 6360.0U1 - Database Design/project_1/data_source/books.csv'
     INTO TABLE BOOK
   FIELDS TERMINATED BY '\t'
 ENCLOSED BY ''
    LINES TERMINATED BY '\r\n'
   IGNORE 1 ROWS
   (Isbn, @dummy, Title, @dummy, @dummy, @dummy, @dummy);

LOAD DATA LOCAL INFILE 'C:/Users/ThinkPad-E531/Documents/Assignments/2016/Summer/CS 6360.0U1 - Database Design/project_1/data_source/book_copies.csv'
     INTO TABLE BOOK_COPIES
   FIELDS TERMINATED BY '\t'
 ENCLOSED BY ''
    LINES TERMINATED BY '\r\n'
   IGNORE 1 ROWS
   (Isbn, Branch_id, No_of_Copies);

LOAD DATA LOCAL INFILE 'C:/Users/ThinkPad-E531/Documents/Assignments/2016/Summer/CS 6360.0U1 - Database Design/project_1/data_source/books.csv'
     INTO TABLE AUTHORS
   FIELDS TERMINATED BY '\t'
 ENCLOSED BY ''
    LINES TERMINATED BY '\r\n'
   IGNORE 1 ROWS
   (@dummy, @dummy, @dummy, Name, @dummy, @dummy, @dummy);

LOAD DATA LOCAL INFILE 'C:/Users/ThinkPad-E531/Documents/Assignments/2016/Summer/CS 6360.0U1 - Database Design/project_1/data_source/books.csv'
     INTO TABLE BOOK_AUTHORS
   FIELDS TERMINATED BY '\t'
 ENCLOSED BY ''
    LINES TERMINATED BY '\r\n'
   IGNORE 1 ROWS
   (Isbn, @dummy, @dummy, @name, @dummy, @dummy, @dummy)
      SET Author_id = (SELECT Author_id
                      FROM AUTHORS
                      WHERE Name = @name);

### Deal with noise ###
UPDATE BOOK
SET    TITLE = REPLACE(TITLE, '&amp;', '&')
WHERE  TITLE LIKE '%&amp;%';

### Show Data ###
SELECT * FROM BOOK LIMIT 10;
SELECT * FROM AUTHORS LIMIT 10;
SELECT * FROM BOOK_AUTHORS LIMIT 10;
SELECT * FROM LIBRARY_BRANCH LIMIT 10;
SELECT * FROM BOOK_COPIES LIMIT 10;
SELECT * FROM BORROWER LIMIT 10;
SELECT * FROM BOOK_LOANS LIMIT 10;
SELECT * FROM FINES LIMIT 10;
