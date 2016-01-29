#-------------------------------------------------
#
# Project created by QtCreator 2015-10-12T11:18:06
#
#-------------------------------------------------

QT       += gui

TARGET = ../BoxFilter
TEMPLATE = lib
CONFIG += plugin

DEFINES += BOXFILTER_LIBRARY

SOURCES += boxfilter.cpp

HEADERS += boxfilter.h\
        boxfilter_global.h \
    ../../filterinterface.h \
    ../../io.h \
    ../../matrix.h

INCLUDEPATH += ../..

unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
