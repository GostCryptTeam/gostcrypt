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
    GC_window.cpp \
    GC_button.cpp \
    GC_volumelist.cpp \
    GC_graphicuserinterface.cpp \
    GC_openvolume.cpp

HEADERS  += \
    GC_window.h \
    GC_button.h \
    GC_volumelist.h \
    GC_graphicuserinterface.h \
    GC_openvolume.h

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
