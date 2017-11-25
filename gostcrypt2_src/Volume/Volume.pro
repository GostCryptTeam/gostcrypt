#-------------------------------------------------
#
# Project created by QtCreator 2017-07-17T17:26:10
#
#-------------------------------------------------

QT       -= gui

TARGET = Volume
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += .. ../Crypto
QMAKE_CXXFLAGS += $(shell pkg-config fuse --cflags) -fno-strict-aliasing

DEFINES += _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE _LARGE_FILES QT_NO_FOREACH
unix {
    DEFINES += GST_UNIX
    linux-g++ {
        DEFINES += GST_LINUX
    }
}

DEFINES += QT_DEPRECATED_WARNINGS

unix {
    target.path = /usr/lib
    INSTALLS += target
}

HEADERS += \
    VolumeSlot.h \
    VolumePasswordCache.h \
    VolumePassword.h \
    VolumeLayout.h \
    VolumeHeader.h \
    Volume.h \
    Version.h \
    Pkcs5Kdf.h \
    Keyfile.h \
    EncryptionThreadPool.h \
    EncryptionTest.h \
    EncryptionModeXTS.h \
    EncryptionMode.h \
    EncryptionAlgorithm.h \
    Crc32.h \
    Cipher.h \
    ../Crypto/Whirlpool.h \
    ../Crypto/Stribog.h \
    ../Crypto/GrasshopperCipher.h \
    ../Crypto/GostHash.h \
    ../Crypto/GostCipher.h \
    ../Common/Pkcs5.h \
    ../Common/GfMul.h \
    ../Common/Endian.h \
    ../Common/Crc.h \
    VolumeInformation.h \
    ../Common/Crypto.h \
    VolumePath.h \
    VolumeProtection.h \
    VolumeHostType.h \
    VolumeType.h \
    VolumeHash.h \
    EncryptionAlgorithmGOST.h \
    EncryptionAlgorithmGrasshopper.h

SOURCES += \
    VolumePassword.cpp \
    VolumeLayout.cpp \
    VolumeHeader.cpp \
    Volume.cpp \
    Pkcs5Kdf.cpp \
    Keyfile.cpp \
    EncryptionThreadPool.cpp \
    EncryptionTest.cpp \
    EncryptionModeXTS.cpp \
    EncryptionMode.cpp \
    EncryptionAlgorithm.cpp \
    Cipher.cpp \
    ../Crypto/Whirlpool.c \
    ../Crypto/Stribog.c \
    ../Crypto/GrasshopperCipher.c \
    ../Crypto/GostHash.c \
    ../Crypto/GostCipher.c \
    ../Common/Pkcs5.c \
    ../Common/GfMul.c \
    ../Common/Endian.c \
    ../Common/Crc.c \
    VolumeInformation.cpp \
    VolumeHash.cpp \
    EncryptionAlgorithmGOST.cpp \
    EncryptionAlgorithmGrasshopper.cpp
