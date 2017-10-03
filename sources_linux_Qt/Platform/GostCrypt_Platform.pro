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
    User.h \
    Time.h \
    Thread.h \
    TextReader.h \
    SystemLog.h \
    SystemInfo.h \
    SystemException.h \
    System.h \
    SyncEvent.h \
    StringConverter.h \
    Stream.h \
    SharedVal.h \
    SharedPtr.h \
    SerializerFactory.h \
    Serializer.h \
    Serializable.h \
    PlatformTest.h \
    PlatformBase.h \
    Platform.h \
    Mutex.h \
    MemoryStream.h \
    Memory.h \
    Functor.h \
    ForEach.h \
    Finally.h \
    FilesystemPath.h \
    FileStream.h \
    File.h \
    Exception.h \
    Event.h \
    Directory.h \
    Buffer.h \

SOURCES += \
    Unix/Time.cpp \
    Unix/Thread.cpp \
    Unix/SystemLog.cpp \
    Unix/SystemInfo.cpp \
    Unix/SystemException.cpp \
    Unix/SyncEvent.cpp \
    Unix/Process.cpp \
    Unix/Poller.cpp \
    Unix/Pipe.cpp \
    Unix/Mutex.cpp \
    Unix/FilesystemPath.cpp \
    Unix/File.cpp \
    Unix/Directory.cpp \
    TextReader.cpp \
    StringConverter.cpp \
    SerializerFactory.cpp \
    Serializer.cpp \
    Serializable.cpp \
    PlatformTest.cpp \
    MemoryStream.cpp \
    Memory.cpp \
    FileCommon.cpp \
    Exception.cpp \
    Event.cpp \
    Buffer.cpp \
