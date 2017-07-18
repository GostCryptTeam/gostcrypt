#-------------------------------------------------
#
# Project created by QtCreator 2017-07-17T17:26:10
#
#-------------------------------------------------

QT       -= core gui

TARGET = Core
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += .. ../Crypto
QMAKE_CXXFLAGS += $(shell pkg-config fuse --cflags) -fno-strict-aliasing

DEFINES += _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE _LARGE_FILES
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
    VolumeCreator.h \
    RandomNumberGenerator.h \
    MountOptions.h \
    HostDevice.h \
    FatFormatter.h \
    CoreException.h \
    CoreBase.h \
    Core.h

unix {
    HEADERS += \
    Unix/System.h \
    Unix/MountedFilesystem.h \
    Unix/CoreUnix.h \
    Unix/CoreServiceResponse.h \
    Unix/CoreServiceRequest.h \
    Unix/CoreServiceProxy.h \
    Unix/CoreService.h

    solaris {
        HEADERS += \
        Unix/Solaris/System.h \
        Unix/Solaris/CoreSolaris.h
    }
    macx {
        HEADERS += \
        Unix/MacOSX/System.h \
        Unix/MacOSX/CoreMacOSX.h
    }
    linux {
        HEADERS += \
        Unix/Linux/System.h \
        Unix/Linux/CoreLinux.h
    }
    freebsd {
        HEADERS += \
        Unix/FreeBSD/System.h \
        Unix/FreeBSD/CoreFreeBSD.h
    }
}

SOURCES += \
    VolumeCreator.cpp \
    RandomNumberGenerator.cpp \
    MountOptions.cpp \
    HostDevice.cpp \
    FatFormatter.cpp \
    CoreException.cpp \
    CoreBase.cpp

unix {
    SOURCES += \
    Unix/CoreUnix.cpp \
    Unix/CoreServiceResponse.cpp \
    Unix/CoreServiceRequest.cpp \
    Unix/CoreService.cpp
    solaris {
        SOURCES += Unix/Solaris/CoreSolaris.cpp
    }
    macx {
        SOURCES += Unix/MacOSX/CoreMacOSX.cpp
    }
    linux {
        SOURCES += Unix/Linux/CoreLinux.cpp
    }
    freebsd {
        SOURCES += Unix/FreeBSD/CoreFreeBSD.cpp
    }
}
