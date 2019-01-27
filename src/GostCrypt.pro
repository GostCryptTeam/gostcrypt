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
    FuseService \
    CoreTestsAsRoot \
    CoreTestsAsUser \
    VolumeTests \
    FuseServiceTests

QMAKE_CXX = ccache g++
QMAKE_CXXFLAGS += -O3

UI.depends = \
    Core \
    Volume \
    FuseService

CoreTestsAsUser.depends = \
    Core \
    Volume \
    FuseService

CoreTestsAsRoot.depends = \
    Core \
    Volume \
    FuseService
    
VolumeTests.depends = \
    Volume \

FuseServiceTests.depends = \
    Volume \
    FuseService

HEADERS += \
    volumehosttype.h

