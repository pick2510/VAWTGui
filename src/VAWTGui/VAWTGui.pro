#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T18:35:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VAWTGui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    libvawt.cpp \
    worker.cpp

HEADERS  += mainwindow.h \
    libvawt.h \
    worker.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

CONFIG += c++11

#CONFIG += staticlib

#CONFIG += static

unix|win32: LIBS += -lwiringPi

unix|win32: LIBS += -lmodbus


