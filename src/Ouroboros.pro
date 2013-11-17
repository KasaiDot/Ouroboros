#-------------------------------------------------
#
# Project created by QtCreator 2013-10-21T17:01:58
#
#-------------------------------------------------

QT       += xml core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Ouroboros
TEMPLATE = app

CONFIG += openssl c++11

win32{
    RC_FILE = Ouroboros.rc
    QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
}


SOURCES += main.cpp\
        ouroboros.cpp \
    threadmanager.cpp \
    apimanager.cpp \
    animeentity.cpp \
    animedatabase.cpp \
    queuemanager.cpp \
    queueitem.cpp \
    settings.cpp \
    user.cpp \
    filemanager.cpp \
    guimanager.cpp \
    dialog_settings.cpp \
    dialog_animeinformation.cpp

HEADERS  += ouroboros.h \
    threadmanager.h \
    apimanager.h \
    globals.h \
    animeentity.h \
    animedatabase.h \
    queuemanager.h \
    queueitem.h \
    private.h \
    settings.h \
    user.h \
    filemanager.h \
    guimanager.h \
    dialog_settings.h \
    dialog_animeinformation.h \
    customguielements.h

FORMS    += ouroboros.ui \
    dialog_settings.ui \
    dialog_animeinformation.ui

RESOURCES += \
    Resources.qrc
