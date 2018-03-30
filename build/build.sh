qmake-qt5 ../gostcrypt2_src/GostCrypt.pro -spec linux-g++ CONFIG+=debug && /usr/bin/make qmake_all > preparation.log
echo "Preparation done."
make clean > /dev/null
echo "Clean Complete."
echo "Starting Compilation..."
make -j $(cat /proc/cpuinfo|grep processor | wc -l) | tee make.log
echo "Compilation done"
