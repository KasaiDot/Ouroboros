#-------------------------------------------------
#
# Project created by QtCreator 2013-10-21T17:01:58
#
#-------------------------------------------------

QT       += xml core gui network

#includes
include(api/api.pri)
include(library/library.pri)
include(manager/manager.pri)
include(ouroboros/ouroboros.pri)
include(recognition/recognition.pri)
include(ui/ui.pri)


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
    singleapplication.cpp \

HEADERS  += singleapplication.h \

RESOURCES += \
    Resources.qrc
