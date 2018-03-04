#!/bin/bash

gcc -o test.out tester.c ../gostcrypt2_src/Volume/Crypto/GostCipher.c ../gostcrypt2_src/Volume/Crypto/GrasshopperCipher.c ../gostcrypt2_src/Volume/Crypto/Stribog.c -I./customQt -O3 -lpthread


#includes and libs for the real Qt 5 on my computer
#-I/opt/Qt/5.9/gcc_64/include/QtCore -I/opt/Qt/5.9/gcc_64/include -L/opt/Qt/5.9/gcc_64/lib
