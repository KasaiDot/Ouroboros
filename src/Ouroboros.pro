#-------------------------------------------------
#
# Project created by QtCreator 2013-10-21T17:01:58
#
#-------------------------------------------------

QT       += xml core gui network

#includes
include(dialogs/dialogs.pri);
include(managers/managers.pri);

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Ouroboros
TEMPLATE = app

CONFIG += openssl c++11

win32{
    RC_FILE = Ouroboros.rc
    QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
    LIBS += -lpsapi
}


SOURCES += main.cpp\
    ouroboros.cpp \
    animeentity.cpp \
    animedatabase.cpp \
    queueitem.cpp \
    settings.cpp \
    user.cpp \
    singleapplication.cpp \
    animeepisode.cpp \
    recognitionengine.cpp \
    common.cpp \

HEADERS  += ouroboros.h \
    globals.h \
    animeentity.h \
    animedatabase.h \
    queueitem.h \
    private.h \
    settings.h \
    user.h \
    customguielements.h \
    singleapplication.h \
    animeepisode.h \
    recognitionengine.h \
    common.h \
    ouroborosdebug.h \
    appinfo.h

FORMS    += ouroboros.ui

RESOURCES += \
    Resources.qrc
