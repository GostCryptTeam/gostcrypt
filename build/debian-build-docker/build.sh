#!/bin/sh
mkdir build-debian
cd build-debian
qmake ../src/GostCrypt.pro
make -j `cat /proc/cpuinfo | grep processor | wc -l` sub-UI
fpm -s dir -t deb -n gostcrypt --version $APPVEYOR_BUILD_VERSION --license GPLv3 -m hantoine02@gmail.com -d qt5-default -d libqt5qml5 -d libqt5quick5 -d libblkid1 -d libfuse2 --deb-no-default-config-files GostCrypt=/usr/bin/gostcrypt
