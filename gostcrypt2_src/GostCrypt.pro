TEMPLATE = subdirs

SUBDIRS = \
    Core \
    Platform \
    Volume \
    UI \
    FuseService

QMAKE_CXX = ccache g++

UI.depends = \
    Core \
    Platform \
    Volume \
    FuseService

HEADERS += \
    volumehosttype.h

