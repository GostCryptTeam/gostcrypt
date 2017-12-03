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
    VolumePassword.h \
    VolumeLayout.h \
    VolumeHeader.h \
    Volume.h \
    Version.h \
    Keyfile.h \
    EncryptionThreadPool.h \
    EncryptionTest.h \
    EncryptionModeXTS.h \
    EncryptionMode.h \
    EncryptionAlgorithm.h \
    Crc32.h \
    VolumeInformation.h \
    VolumeProtection.h \
    VolumeType.h \
    VolumeHash.h \
    EncryptionAlgorithmGOST.h \
    EncryptionAlgorithmGrasshopper.h \
    CipherAlgorithm.h \
    CipherAlgorithmGOST.h \
    CipherAlgorithmGrasshopper.h \
    VolumeHashWhirlpool.h \
    VolumeHashGostHash.h \
    VolumeHashStribog.h \
    VolumeLayoutV2Normal.h \
    VolumeLayoutV2Hidden.h \
    VolumeFile.h \
    ../Common/Crc.h \
    ../Common/Crypto.h \
    ../Common/GfMul.h \
    ../Common/Gstdefs.h \
    ../Common/Pkcs5.h \
    ../Common/Random.h \
    ../Common/Volumes.h \
    ../Crypto/GostCipher.h \
    ../Crypto/GostHash.h \
    ../Crypto/GrasshopperCipher.h \
    ../Crypto/Stribog.h \
    ../Crypto/Whirlpool.h \
    VolumeException.h

SOURCES += \
    VolumePassword.cpp \
    VolumeLayout.cpp \
    VolumeHeader.cpp \
    Volume.cpp \
    Keyfile.cpp \
    EncryptionThreadPool.cpp \
    EncryptionTest.cpp \
    EncryptionModeXTS.cpp \
    EncryptionMode.cpp \
    EncryptionAlgorithm.cpp \
    VolumeInformation.cpp \
    VolumeHash.cpp \
    EncryptionAlgorithmGOST.cpp \
    EncryptionAlgorithmGrasshopper.cpp \
    CipherAlgorithmGOST.cpp \
    CipherAlgorithmGrasshopper.cpp \
    VolumeHashWhirlpool.cpp \
    VolumeHashGostHash.cpp \
    VolumeHashStribog.cpp \
    CipherAlgorithm.cpp \
    VolumeLayoutV2Normal.cpp \
    VolumeLayoutV2Hidden.cpp \
    VolumeFile.cpp \
    ../Common/Crc.c \
    ../Common/GfMul.c \
    ../Common/Pkcs5.c \
    ../Crypto/GostCipher.c \
    ../Crypto/GostHash.c \
    ../Crypto/GrasshopperCipher.c \
    ../Crypto/Stribog.c \
    ../Crypto/Whirlpool.c \
    VolumeException.cpp
