#-------------------------------------------------
#
# Project created by QtCreator 2017-07-17T15:29:05
#
#-------------------------------------------------

QT       -= core gui

TARGET = FuseDriver
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += .. ../Crypto
QMAKE_CXXFLAGS += $(shell pkg-config fuse --cflags)

DEFINES += _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE _LARGE_FILES
unix {
    DEFINES += GST_UNIX
    linux-g++ {
        DEFINES += GST_LINUX
    }
}

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    FuseService.cpp

HEADERS += \
    FuseService.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
