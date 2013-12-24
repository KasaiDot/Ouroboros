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
}


SOURCES += main.cpp\
    mainupdatewindow.cpp \
    updateperformclass.cpp \
    updateavailiabledialog.cpp

HEADERS  += \
    mainupdatewindow.h \
    updateperformclass.h \
    Globals.h \
    ServerInfo.h \
    updateavailiabledialog.h

FORMS    += \
    mainupdatewindow.ui \
    updateavailiabledialog.ui

