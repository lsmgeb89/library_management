#-------------------------------------------------
#
# Project created by QtCreator 2016-07-01T22:00:59
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = library_management
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dbmanager.cpp \
    branchdialog.cpp

HEADERS  += mainwindow.h \
    dbmanager.h \
    branchdialog.h

FORMS    += mainwindow.ui \
    branchdialog.ui
