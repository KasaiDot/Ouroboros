#-------------------------------------------------
#
# Project created by QtCreator 2014-10-21T17:01:58
#
#-------------------------------------------------

QT += xml core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += openssl c++11

win32{
    RC_FILE = Ouroboros.rc
    QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
    LIBS += -lpsapi
}

TARGET = Ouroboros
TEMPLATE = app

SOURCES += main.cpp\
    singleapplication.cpp

HEADERS  += singleapplication.h

#includes
include(api/api.pri)
include(library/library.pri)
include(manager/manager.pri)
include(ouroboros/ouroboros.pri)
include(recognition/recognition.pri)
include(ui/ui.pri)

RESOURCES += \
    Resources.qrc
