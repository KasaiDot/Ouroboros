#-------------------------------------------------
#
# Project created by QtCreator 2013-06-13T20:30:37
#
#-------------------------------------------------

QT       += xml network core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Updater
TEMPLATE = app

win32{
    RC_FILE = Ouroboros.rc
    QMAKE_LFLAGS += -static-libgcc
    QMAKE_LFLAGS += -static-libstdc++
}


SOURCES += main.cpp\
    mainupdatewindow.cpp \
    updateperformclass.cpp

HEADERS  += \
    mainupdatewindow.h \
    updateperformclass.h \
    Globals.h \
    ServerInfo.h

FORMS    += \
    mainupdatewindow.ui

