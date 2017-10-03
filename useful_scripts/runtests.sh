#!/bin/bash

cleanall () {
	echo "Cleaning Everything..." | tee -a $LOGFILE
	# dismounting everything...
	RESPONSE=$($GOSTCRYPT dismountall | tee -a $LOGFILE)
	if [ "$RESPONSE" != "Volume Dismounted." ]; then
		echo "Unable to dismount-all !" | tee -a $LOGFILE
		echo "Tests can't be resumed.. stopping." | tee -a $LOGFILE
		exit 1
	fi
	
	#checking if dismount was ok...
	RESPONSE=$($GOSTCRYPT list volumes | tee -a $LOGFILE)
	if [ ! -z "$RESPONSE" ]; then
		echo "Volumes still mounted !" | tee -a $LOGFILE
		echo "Tests can't be resumed.. stopping." | tee -a $LOGFILE
		exit 1
	fi
}

GOSTCRYPT=../build-GostCrypt-Desktop_Qt_5_9_0_GCC_64bit-Debug/GostCrypt
LOGFILE=last.log
MOUNTPATH=/media/$USER/gostcrypt1

# erasing last.log
echo "Starting all test cases..." | tee $LOGFILE

$GOSTCRYPT list algorithms 	> algolist.txt
RESPONSE=$(diff algolist.txt verified/algolist.txt | tee -a $LOGFILE)
if [[ -z $RESPONSE ]]; then
	echo "Algorithm list OK.." | tee -a $LOGFILE
else
	echo "Algorithm list changed or wrong !" | tee -a $LOGFILE
fi

$GOSTCRYPT list hashs		> hashlist.txt
RESPONSE=$(diff hashlist.txt verified/hashlist.txt | tee -a $LOGFILE)
if [[ -z $RESPONSE ]]; then
	echo "Derivation functions list OK.." | tee -a $LOGFILE
else
	echo "Derivation functions list changed or wrong !" | tee -a $LOGFILE
fi

#can't test this, it's too system dependant
#$GOSTCRYPT list filesystems	> filesystemlist.txt

# can't test this
#$GOSTCRYPT list devices		> devicelist.txt

# cleaning before getting to some serious tests
cleanall

echo "Creating volume.." | tee -a $LOGFILE
$GOSTCRYPT create -p pass --filesystem vfat volume >> $LOGFILE
echo "Mounting volume.." | tee -a $LOGFILE
$GOSTCRYPT mount -p pass -m $MOUNTPATH -g root volume >> $LOGFILE
echo "Listing volume.." | tee -a $LOGFILE
ls -l $MOUNTPATH
$GOSTCRYPT list volumes

cleanall



