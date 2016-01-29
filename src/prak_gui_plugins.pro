#-------------------------------------------------
#
# Project created by QtCreator 2015-10-11T11:28:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = prak_gui_plugins
TEMPLATE = app


SOURCES += main_gui.cpp\
        mainwindow.cpp \
    io.cpp \
    controller_gui.cpp \
    model.cpp \
    view_gui.cpp

HEADERS  += mainwindow.h \
    matrix.h \
    io.h \
    matrix.hpp \
    controller_gui.h \
    MyObject.h \
    model.h \
    view_gui.h \
    filterinterface.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
