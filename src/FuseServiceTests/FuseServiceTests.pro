QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../ ../Crypto/ ../FuseService

SOURCES +=  \
    main.cpp \

LIBS += \
        ../FuseService/libFuseService.a \
        ../Volume/libVolume.a \
        -ldl \
        -pthread \
        -lfuse \
        -lblkid

TARGET = ../testFuseService
