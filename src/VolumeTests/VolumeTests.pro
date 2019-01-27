QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../ ../Crypto/ ../Volume

SOURCES +=  \
    main.cpp \

LIBS += \
        ../Volume/libVolume.a \
        -ldl \
        -pthread
