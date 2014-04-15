#-------------------------------------------------
#
# Project created by QtCreator 2014-04-01T09:32:34
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    GView.cpp

HEADERS  += mainwindow.h \
    GView.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
