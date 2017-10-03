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
    VolumeInfo.h \
    VolumeHeader.h \
    VolumeException.h \
    Volume.h \
    Version.h \
    Pkcs5Kdf.h \
    Keyfile.h \
    Hash.h \
    EncryptionThreadPool.h \
    EncryptionTest.h \
    EncryptionModeXTS.h \
    EncryptionModeLRW.h \
    EncryptionModeCBC.h \
    EncryptionMode.h \
    EncryptionAlgorithm.h \
    Crc32.h \
    Cipher.h \
    ../Crypto/Whirlpool.h \
    ../Crypto/Stribog.h \
    ../Crypto/GrasshopperCipher.h \
    ../Crypto/GostHash.h \
    ../Crypto/GostCipher.h \
    ../Common/SecurityToken.h \
    ../Common/Pkcs5.h \
    ../Common/GfMul.h \
    ../Common/Endian.h \
    ../Common/Crc.h

SOURCES += \
    VolumePasswordCache.cpp \
    VolumePassword.cpp \
    VolumeLayout.cpp \
    VolumeInfo.cpp \
    VolumeHeader.cpp \
    VolumeException.cpp \
    Volume.cpp \
    Pkcs5Kdf.cpp \
    Keyfile.cpp \
    Hash.cpp \
    EncryptionThreadPool.cpp \
    EncryptionTest.cpp \
    EncryptionModeXTS.cpp \
    EncryptionModeLRW.cpp \
    EncryptionModeCBC.cpp \
    EncryptionMode.cpp \
    EncryptionAlgorithm.cpp \
    Cipher.cpp \
    ../Crypto/Whirlpool.c \
    ../Crypto/Stribog.c \
    ../Crypto/GrasshopperCipher.c \
    ../Crypto/GostHash.c \
    ../Crypto/GostCipher.c \
    ../Common/SecurityToken.cpp \
    ../Common/Pkcs5.c \
    ../Common/GfMul.c \
    ../Common/Endian.c \
    ../Common/Crc.c
