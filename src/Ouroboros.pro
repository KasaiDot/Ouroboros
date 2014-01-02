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

HEADERS  += singleapplication.h \
    AutoTest.h

#DEFINES += APP_DEBUG=false APP_UNITTEST=false

#includes
include(api/api.pri)
include(library/library.pri)
include(manager/manager.pri)
include(ouroboros/ouroboros.pri)
include(recognition/recognition.pri)
include(ui/ui.pri)

#Add "Config+=UnitTest" as an additional argument to qmake
UnitTest
{
    QT += testlib
    include(tests/tests.pri)
}


RESOURCES += \
    Resources.qrc
