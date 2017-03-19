#-------------------------------------------------
#
# Project created by QtCreator 2017-02-14T17:31:47
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GostCrypt
TEMPLATE = app


SOURCES += main.cpp\
    Window.cpp \
    Button.cpp \
    VolumeList.cpp \
    GraphicUserInterface.cpp \
    OpenVolumeDialog.cpp

HEADERS  += \
    Window.h \
    Button.h \
    VolumeListDialog.h \
    GraphicUserInterface.h \
    OpenVolumeDialog.h

FORMS    +=

RESOURCES += \
    ressources.qrc

LIBS += ../Platform/Platform.a \
    ../Volume/Volume.a \
    ../Driver/Fuse/Driver.a \
    ../Core/Core.a \
    -ldl \
    -pthread \
    -lfuse

TRANSLATIONS = GostCrypt_en.ts GostCrypt_ru.ts
