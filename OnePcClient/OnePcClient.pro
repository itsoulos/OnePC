#-------------------------------------------------
#
# Project created by QtCreator 2015-07-06T18:26:41
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OnePcClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    problem.cpp \
    myproblem.cpp \
    population.cpp \
    plotarea.cpp \
    mythread.cpp \
    global.cpp \
    clientprotocol.cpp

HEADERS  += mainwindow.h \
    problem.h \
    myproblem.h \
    population.h \
    plotarea.h \
    mythread.h \
    global.h \
    protocol.h \
    clientprotocol.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

DISTFILES += \
    GaClient.pro.user \
    GaClient.pro.user.b5f1467 \
    GaClient.pro.user.ff93a13 \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

