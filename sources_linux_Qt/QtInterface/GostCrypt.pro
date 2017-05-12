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
    OpenVolumeDialog.cpp \

HEADERS  += Window.h \
    Button.h \
    VolumeList.h \
    GraphicUserInterface.h \
    OpenVolumeDialog.h \

FORMS    +=

RESOURCES += \
    ressources.qrc

#LIBS += ../Core/Core.a \
#    ../Volume/Volume.a \
#    ../Platform/Platform.a \
#    ../Driver/Fuse/Driver.a \
#    -ldl \
#    -pthread \
#   -lfuse

INCLUDEPATH += ../ \
    ../Crypto/ \
    ../Volume/

TRANSLATIONS = GostCrypt_en.ts GostCrypt_ru.ts
