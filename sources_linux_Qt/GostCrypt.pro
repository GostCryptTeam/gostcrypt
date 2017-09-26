TEMPLATE = subdirs

SUBDIRS = \
    GostCrypt_NewCore \
    GostCrypt_Platform \
    GostCrypt_FuseDriver \
    GostCrypt_Volume \
    GostCrypt_UI

QMAKE_CXX = ccache g++

GostCrypt_FuseDriver.file = FuseDriver/GostCrypt_FuseDriver.pro
GostCrypt_Platform.file = Platform/GostCrypt_Platform.pro
GostCrypt_Volume.file = Volume/GostCrypt_Volume.pro
GostCrypt_UI.file = UI/GostCrypt_UI.pro
GostCrypt_NewCore.file = NewCore/GostCrypt_NewCore.pro

GostCrypt_UI.depends = \
    GostCrypt_NewCore \
    GostCrypt_Platform \
    GostCrypt_Volume \
    GostCrypt_FuseDriver

