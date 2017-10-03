echo "Starting Compilation..."
cd dircompile/
make clean > /dev/null
echo "Clean Complete."
cd ..
./prepare.sh > dircompile/preparation.log
echo "Preparation done."
cd dircompile/
make -j 8 | tee making.log
echo "Compilation done"
