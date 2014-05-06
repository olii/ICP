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
    GView.cpp \
    gpixmapitem.cpp \
    map.cpp

HEADERS  += mainwindow.h \
    GView.h \
    gpixmapitem.h \
    map.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

win32 {
    INCLUDEPATH += C:/Users/olii/Desktop/boost_1_48_0
    LIBS += C:/Users/olii/Desktop/boost_1_48_0/stage/lib/libboost_serialization-mgw48-mt-d-1_48.a
}
linux {
    LIBS += -lboost_serialization
}
