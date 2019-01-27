TEMPLATE = subdirs

lessThan(QT_MAJOR_VERSION, 5) {
	error("Compilation requires at least a Qt version of 5.7")
}
lessThan(QT_MAJOR_VERSION, 6) : lessThan(QT_MINOR_VERSION, 7) { 
	error("Compilation requires at least a Qt version of 5.7")
}

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

