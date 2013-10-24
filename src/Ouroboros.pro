#-------------------------------------------------
#
# Project created by QtCreator 2013-10-21T17:01:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Ouroboros
TEMPLATE = app

CONFIG += openssl c++11


SOURCES += main.cpp\
        ouroboros.cpp \
    threadmanager.cpp \
    apimanager.cpp \
    animeentity.cpp \
    animedatabase.cpp \
    queuemanager.cpp \
    queueitem.cpp \
    queueworker.cpp

HEADERS  += ouroboros.h \
    threadmanager.h \
    apimanager.h \
    globals.h \
    animeentity.h \
    animedatabase.h \
    queuemanager.h \
    queueitem.h \
    queueworker.h \
    private.h

FORMS    += ouroboros.ui
