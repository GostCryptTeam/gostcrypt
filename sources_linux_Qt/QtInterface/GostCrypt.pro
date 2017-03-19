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
    wizard/Container_Type.cpp \
    wizard/Encryption_Choice.cpp \
    wizard/Format.cpp \
    wizard/Location.cpp \
    wizard/Password.cpp \
    wizard/Size.cpp \
    wizard/Ui_CreationWizard.cpp

HEADERS  += \
    Window.h \
    Button.h \
    VolumeList.h \
    GraphicUserInterface.h \
    OpenVolumeDialog.h
    wizard/Container_Type.h \
    wizard/Encryption_Choice.h \
    wizard/Format.h \
    wizard/Location.h \
    wizard/Password.h \
    wizard/Size.h \
    wizard/Ui_CreationWizard.h

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
