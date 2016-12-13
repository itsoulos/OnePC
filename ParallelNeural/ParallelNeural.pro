#-------------------------------------------------
#
# Project created by QtCreator 2016-11-24T08:21:32
#
#-------------------------------------------------

QT       -= gui

TARGET = ParallelNeural
TEMPLATE = lib

DEFINES += PARALLELNEURAL_LIBRARY

SOURCES += parallelneural.cpp \
    tolmin.cc

HEADERS += parallelneural.h\
        parallelneural_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
