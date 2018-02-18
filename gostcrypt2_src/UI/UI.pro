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
    TranslationApp.h \
    Parser.h \
    CmdLineInterface.h \
    GraphicInterface.h \
    SecureTextField.h \
    UserInterface.h

SOURCES += \
    main.cpp \
    UserSettings.cpp \
    TranslationApp.cpp \
    Parser.cpp \
    CmdLineInterface.cpp \
    GraphicInterface.cpp \
    SecureTextField.cpp \
    UserInterface.cpp

LIBS += \
        ../Core/libCore.a \
        ../FuseService/libFuseService.a \
        ../Volume/libVolume.a \
        -ldl \
        -pthread \
        -lfuse \
        -lblkid

TARGET = ../GostCrypt

PRE_TARGETDEPS += ../Core/libCore.a \
    ../Volume/libVolume.a \
    ../FuseService/libFuseService.a

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
    UI/SecureTextField.qml \
    UI/wizard/StepComponent.qml \
    UI/SubWindow.qml \
    UI/wizard/NextPreviousButton.qml \
    UI/dialogs/FavoriteVolumeOptions.qml \
    UI/dialogs/OpenVolume.qml \
    UI/dialogs/Performance.qml \
    UI/dialogs/Preferences.qml \
    UI/dialogs/DefaultKeyfile.qml \
    UI/dialogs/Contributors.qml \
    UI/dialogs/ChangePassword.qml \
    UI/dialogs/Benchmark.qml \
    UI/dialogs/License.qml \
    UI/dialogs/KeyfileGenerator.qml \
    UI/dialogs/TestVectors.qml \
    translations/gostcrypt_ar.ts \
    UI/dialogs/RestoreHeaderVolume.qml \
    UI/dialogs/BackupHeaderVolume.qml \
    UI/AbstractTheme.qml
