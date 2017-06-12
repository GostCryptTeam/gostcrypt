TEMPLATE = app

QT += qml quick
CONFIG += c++11
TARGET = GostCrypt

SOURCES += main.cpp \
    modelclass.cpp \
    volumeitemgraphic.cpp \
    GraphicUserInterface.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    modelclass.h \
    volumeitemgraphic.h \
    GraphicUserInterface.h

LIBS += ../Core/Core.a \
    ../Volume/Volume.a \
    ../Platform/Platform.a \
    ../Driver/Fuse/Driver.a \
    -ldl \
    -pthread \
   -lfuse

INCLUDEPATH += ../ \
    ../Crypto/ \
    ../Volume/

#TRANSLATIONS = GostCrypt_en.ts GostCrypt_ru.ts
