#-------------------------------------------------
#
# Project created by QtCreator 2017-07-17T17:26:10
#
#-------------------------------------------------

QT += qml quick

TARGET = GostCrypt
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += ../ ../Crypto/

DEFINES += _FILE_OFFSET_BITS=64 \
    _LARGEFILE_SOURCE \
    _LARGE_FILES

unix {
    DEFINES += GST_UNIX
    linux {
        DEFINES += GST_LINUX
    }
    macx {
        DEFINES += GST_BSD GST_MACOSX
    }
    freebsd {
        DEFINES += GST_BSD GST_FREEBSD
    }
    solaris {
        DEFINES += GST_SOLARIS
    }
}

DEFINES += QT_DEPRECATED_WARNINGS QT_NO_FOREACH


RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    GraphicUserInterface.h \
    connectSignals.h \
    UserSettings.h \
    DragWindowProvider.h \
    volumecreation.h

SOURCES += main.cpp \
    GraphicUserInterface.cpp \
    connectSignals.cpp \
    UserSettings.cpp \
    volumecreation.cpp

LIBS += ../Core/libCore.a \
    ../Volume/libVolume.a \
    ../Platform/libPlatform.a \
    ../FuseDriver/libFuseDriver.a \
    -ldl \
    -pthread \
   -lfuse

TARGET = ../GostCrypt

PRE_TARGETDEPS += ../Core/libCore.a \
    ../Volume/libVolume.a \
    ../Platform/libPlatform.a \
    ../FuseDriver/libFuseDriver.a

TRANSLATIONS = gostcrypt_en.ts gostcrypt_fr.ts gostcrypt_es.ts gostcrypt_it.ts gostcrypt_ru.ts gostcrypt_ar.ts
translation {
SOURCES += UI/* \
           UI/dialogs/* \
           UI/frames/* \
           UI/ressource/* \
           UI/wizard/* \
}

DISTFILES +=
