TEMPLATE = subdirs

SUBDIRS = \
    GostCrypt_Core \
    GostCrypt_FuseDriver \
    GostCrypt_Platform \
    GostCrypt_Volume \
    GostCrypt_UI

GostCrypt_Core.file = Core/GostCrypt_Core.pro
GostCrypt_FuseDriver.file = FuseDriver/GostCrypt_FuseDriver.pro
GostCrypt_Platform.file = Platform/GostCrypt_Platform.pro
GostCrypt_Volume.file = Volume/GostCrypt_Volume.pro
GostCrypt_UI.file = UI/GostCrypt_UI.pro

UI.depends = \
    GostCrypt_Core \
    GostCrypt_FuserDriver \
    GostCrypt_Platform \
    GostCrypt_Volume \
    GostCrypt_UI
