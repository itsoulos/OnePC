#-------------------------------------------------
#
# Project created by QtCreator 2016-11-22T19:39:12
#
#-------------------------------------------------

QT       -= gui

TARGET = potential
TEMPLATE = lib

DEFINES += POTENTIAL_LIBRARY

SOURCES += potential.cpp

HEADERS += potential.h\
        potential_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
