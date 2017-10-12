#-------------------------------------------------
#
# Project created by QtCreator 2017-07-17T17:26:10
#
#-------------------------------------------------

QT += qml quick widgets

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
    UserSettings.h \
    DragWindowProvider.h \
    volumecreation.h \
    TranslationApp.h \
    Parser.h \
    CmdLineInterface.h \
    GraphicInterface.h \
    SecureTextField.h

SOURCES += main.cpp \
    UserSettings.cpp \
    volumecreation.cpp \
    TranslationApp.cpp \
    Parser.cpp \
    CmdLineInterface.cpp \
    GraphicInterface.cpp \
    SecureTextField.cpp

LIBS += \
        ../NewCore/libNewCore.a \
        ../FuseDriver/libFuseDriver.a \
        ../Volume/libVolume.a \
        ../Platform/libPlatform.a \
        -ldl \
        -pthread \
        -lfuse \
        -lblkid

TARGET = ../GostCrypt

PRE_TARGETDEPS += ../NewCore/libNewCore.a \
    ../Volume/libVolume.a \
    ../Platform/libPlatform.a \
    ../FuseDriver/libFuseDriver.a

lupdate_hack{
    SOURCES += qml/*.qml \
        qml/*.js
}
TRANSLATIONS =  translations/gostcrypt_en.ts \
                translations/gostcrypt_fr.ts \
                translations/gostcrypt_es.ts \
                translations/gostcrypt_it.ts \
                translations/gostcrypt_ru.ts \
                translations/gostcrypt_ar.ts

translation {
    SOURCES += UI/* \
               UI/dialogs/* \
               UI/frames/* \
               UI/ressource/* \
               UI/wizard/* \
}

DISTFILES += \
    UI/ressource/separator.png \
    UI/dialogs/GSLanguage.qml \
    UI/SecureTextField.qml


