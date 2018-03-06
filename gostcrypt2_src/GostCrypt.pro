TEMPLATE = subdirs

SUBDIRS = \
    Core \
    Volume \
    UI \
    FuseService

QMAKE_CXX = ccache g++
QMAKE_CXXFLAGS += -O3

UI.depends = \
    Core \
    Volume \
    FuseService

HEADERS += \
    volumehosttype.h

