#-------------------------------------------------
#
# Project created by QtCreator 2015-07-06T16:43:10
#
#-------------------------------------------------

QT       += core gui network sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OnePcServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gaserver.cpp \
    gathread.cpp \
    database.cpp \
    osdialog.cpp \
    aboutproblemdialog.cpp \
    parameterdialog.cpp \
    clientchartdialog.cpp \
    parser.cpp

HEADERS  += mainwindow.h \
    gaserver.h \
    gathread.h \
    client.h \
    protocol.h \
    database.h \
    osdialog.h \
    aboutproblemdialog.h \
    parameterdialog.h \
    clientchartdialog.h \
    parser.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    resources.qrc \
    style.qrc
addFiles.sources=
