TEMPLATE = app

QT += qml quick
CONFIG += c++11
TARGET = GostCrypt

DEFINES += GST_UNIX \
    GST_LINUX

SOURCES += main.cpp \
    GraphicUserInterface.cpp \
    connectSignals.cpp \
    UserSettings.cpp \
    volumecreation.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

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

LIBS += ../Core/Core.a \
    ../Volume/Volume.a \
    ../Platform/Platform.a \
    ../Driver/Fuse/Driver.a \
    -ldl \
    -pthread \
   -lfuse

PRE_TARGETDEPS += ../Core/Core.a \
    ../Volume/Volume.a \
    ../Platform/Platform.a \
    ../Driver/Fuse/Driver.a

INCLUDEPATH += ../ \
    ../Crypto/ \
    ../Volume/

#TRANSLATIONS = GostCrypt_en.ts GostCrypt_ru.ts

DISTFILES += \
