qmake-qt5 ../sources_linux_Qt/GostCrypt.pro -spec linux-g++ CONFIG+=debug && /usr/bin/make qmake_all > preparation.log
echo "Preparation done."
make clean > /dev/null
echo "Clean Complete."
echo "Starting Compilation..."
make -j 8 | tee make.log
echo "Compilation done"
