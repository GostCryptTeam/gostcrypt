TEMPLATE = subdirs

lessThan(QT_VERSION, 5.7): error("Compilation requires at least a Qt version of 5.7")

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

