TEMPLATE = subdirs

SUBDIRS = \
    NewCore \
    Platform \
    FuseDriver \
    Volume \
    UI

QMAKE_CXX = ccache g++

UI.depends = \
    NewCore \
    Platform \
    Volume \
    FuseDriver


