QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../ ../Crypto/ ../Core

SOURCES +=  \
    main.cpp \
    tst_CoreUser.cpp

LIBS += \
        ../Core/libCore.a \
        ../FuseService/libFuseService.a \
        ../Volume/libVolume.a \
        -ldl \
        -pthread \
        -lfuse \
        -lblkid

