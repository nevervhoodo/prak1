#-------------------------------------------------
#
# Project created by QtCreator 2015-10-12T11:18:06
#
#-------------------------------------------------

QT       += gui

TARGET = ../MedianFilter
TEMPLATE = lib
CONFIG += plugin

DEFINES += MEDIANFILTER_LIBRARY

SOURCES += medianfilter.cpp

HEADERS += medianfilter.h\
        medianfilter_global.h \
    ../../filterinterface.h \
    ../../io.h \
    ../../matrix.h

INCLUDEPATH += ../..

unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
