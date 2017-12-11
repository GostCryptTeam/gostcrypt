TEMPLATE = subdirs

SUBDIRS = \
    Core \
    Volume \
    UI \
    FuseService

QMAKE_CXX = ccache g++

UI.depends = \
    Core \
    Volume \
    FuseService

HEADERS += \
    volumehosttype.h

