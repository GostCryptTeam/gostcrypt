TEMPLATE = subdirs

SUBDIRS = \
    NewCore \
    Platform \
    Volume \
    UI \
    NewFuseService

QMAKE_CXX = ccache g++

UI.depends = \
    NewCore \
    Platform \
    Volume \
    NewFuseService

