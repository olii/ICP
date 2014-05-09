#-------------------------------------------------
#
# Project created by QtCreator 2014-05-07T13:28:52
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11 -Wno-unused-local-typedefs
CONFIG += exceptions

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = klient
TEMPLATE = app


SOURCES += gui-main.cpp\
        mainwindow.cpp \
    game.cpp \
    ../../shared/command.cpp \
    ../../shared/map.cpp \
    ../../shared/serverinfo.cpp \
    ../network.cpp \
    informator.cpp \
    gpixmapitem.cpp \
    GView.cpp

HEADERS  += mainwindow.h \
    game.h \
    ../../shared/command.h \
    ../../shared/global.h \
    ../../shared/map.h \
    ../../shared/serverinfo.h \
    ../network.h \
    informator.h \
    gpixmapitem.h \
    GView.h

FORMS    += mainwindow.ui \
    game.ui

win32 {
    INCLUDEPATH += C:/boost_1_55_0
    LIBS += C:/boost_1_55_0/stage/lib/libboost_serialization-mgw48-mt-d-1_55.a
    LIBS += C:/boost_1_55_0/stage/lib/libboost_system-mgw48-mt-d-1_55.a
    LIBS += -lws2_32
}
linux {
    LIBS += -lboost_system -lboost_serialization -lboost_filesystem
}

RESOURCES += \
    resources.qrc
