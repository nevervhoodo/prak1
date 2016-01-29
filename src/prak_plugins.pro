#-------------------------------------------------
#
# Project created by QtCreator 2015-10-12T03:02:16
#
#-------------------------------------------------

QT       += core

QT      += gui

TARGET = prak_plugins
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    controller.cpp \
    io.cpp \
    model.cpp \
    view.cpp

HEADERS += \
    filterinterface.h \
    controller.h \
    io.h \
    matrix.h \
    matrix.hpp \
    model.h \
    MyObject.h \
    view.h

QMAKE_CXXFLAGS += -std=c++11
