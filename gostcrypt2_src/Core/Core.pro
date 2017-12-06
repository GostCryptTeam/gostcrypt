#-------------------------------------------------
#
# Project created by QtCreator 2017-07-17T17:26:10
#
#-------------------------------------------------

QT       -= gui

TARGET = Core
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += .. ../Crypto
QMAKE_CXXFLAGS += $(shell pkg-config fuse --cflags) -fno-strict-aliasing

DEFINES += _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE _LARGE_FILES QT_NO_FOREACH HAVE_NANOSLEEP HAVE_FSTATAT
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
    CoreBase.h \
    CoreResponse.h \
    SerializationUtil.h \
    CoreUser.h \
    CoreRoot.h \
    CoreException.h \
    RandomNumberGenerator.h \
    LoopDeviceManager.h \
    MountFilesystemManager.h \
    loopdevlib/at.h \
    loopdevlib/blkdev.h \
    loopdevlib/c.h \
    loopdevlib/canonicalize.h \
    loopdevlib/linux_version.h \
    loopdevlib/loopdev.h \
    loopdevlib/pathnames.h \
    loopdevlib/sysfs.h \
    CoreServiceHandler.h \
    CoreService.h \
    CoreRequest.h \
    Service.h \
    ServiceHandler.h \
    GostCryptException.h

SOURCES += \
    CoreBase.cpp \
    CoreResponse.cpp \
    CoreUser.cpp \
    CoreRoot.cpp \
    CoreException.cpp \
    RandomNumberGenerator.cpp \
    LoopDeviceManager.cpp \
    MountFilesystemManager.cpp \
    loopdevlib/at.c \
    loopdevlib/blkdev.c \
    loopdevlib/canonicalize.c \
    loopdevlib/linux_version.c \
    loopdevlib/loopdev.c \
    loopdevlib/sysfs.c \
    CoreServiceHandler.cpp \
    CoreService.cpp \
    SerializationUtil.cpp \
    CoreRequest.cpp \
    Service.cpp \
    ServiceHandler.cpp \
    GostCryptException.cpp
