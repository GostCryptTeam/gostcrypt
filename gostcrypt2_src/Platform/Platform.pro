#-------------------------------------------------
#
# Project created by QtCreator 2017-07-17T17:26:10
#
#-------------------------------------------------

QT       -= gui

TARGET = Platform
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += .. ../Crypto
QMAKE_CXXFLAGS += $(shell pkg-config fuse --cflags) -fno-strict-aliasing

DEFINES += _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE _LARGE_FILES QT_NO_FOREACH
unix {
    DEFINES += GST_UNIX
    linux-g++ {
        DEFINES += GST_LINUX
    }
}

DEFINES += QT_DEPRECATED_WARNINGS

unix {
    target.path = /usr/lib
    INSTALLS += target
}

HEADERS += \
    SharedVal.h \
    Platform.h \
    Memory.h \
    File.h \
    Buffer.h \
    FilePath.h

SOURCES += \
    Unix/File.cpp \
    Memory.cpp \
    FileCommon.cpp \
    Buffer.cpp \
    Unix/FilePath.cpp
