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
    Unix/System.h \
    Unix/Process.h \
    Unix/Poller.h \
    Unix/Pipe.h \
    Time.h \
    Thread.h \
    TextReader.h \
    SystemLog.h \
    SystemInfo.h \
    System.h \
    SyncEvent.h \
    Stream.h \
    SharedVal.h \
    PlatformTest.h \
    PlatformBase.h \
    Platform.h \
    Mutex.h \
    MemoryStream.h \
    Memory.h \
    Functor.h \
    Finally.h \
    FileStream.h \
    File.h \
    Buffer.h \
    FilePath.h

SOURCES += \
    Unix/Time.cpp \
    Unix/Thread.cpp \
    Unix/SystemLog.cpp \
    Unix/SyncEvent.cpp \
    Unix/Poller.cpp \
    Unix/Mutex.cpp \
    Unix/File.cpp \
    TextReader.cpp \
    PlatformTest.cpp \
    MemoryStream.cpp \
    Memory.cpp \
    FileCommon.cpp \
    Event.cpp \
    Buffer.cpp \
    Unix/FilePath.cpp
