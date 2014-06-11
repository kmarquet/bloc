#!/bin/sh

#set -o verbose

#HASH_FUNC_DIRS="AES CLEFIA128 CLEFIA192 CLEFIA256 DESXL DIRnoekeon HIGHT IDEA INDnoekeon KATAN32 KATAN48 KATAN64 KLEIN64 KLEIN80 KLEIN96 KTANTAN32 KTANTAN48 KTANTAN64 LBlock LED128 LED128_tcalc LED128_tdur LED64 LED64_tcalc LED64_tdur MCRYPTON64 MCRYPTON96 MCRYPTON128 MIBS64 MIBS80 PRESENT_SIZE PRESENT_SPEED Piccolo128 Piccolo80 SEA SKIPJACK TWINE80 TWINE128 SIMON64_96 SIMON64_128 SIMON96_96 SIMON96_144 SIMON128_128 SPECK64_96 SPECK64_128 SPECK96_96 SPECK96_144 SPECK128_128 LILLIPUT" #PRINCE
HASH_FUNC_DIRS="AES"
#HASH_FUNC_DIRS="KTANTAN64 PRESENT_SPEED"
#HASH_FUNC_DIRS="CLEFIA128 LILLIPUT"

#OPT_LEVEL="-O0 -O1 -O2 -O3 -Os"
OPT_LEVEL="-O3"

#TODO : KATAN_KTANTAN_16bits

BASE_DIR=`pwd`
UTILS_DIR="$BASE_DIR/utils"

RESULT_DIR="$BASE_DIR/results"
rm -rf $RESULT_DIR
mkdir $RESULT_DIR

LOG_FILE="$RESULT_DIR/LOG-hard.txt"
rm -f "$LOG_FILE"
touch "$LOG_FILE"

for level in $OPT_LEVEL
do
    RESULT_FILE="${RESULT_DIR}/MEMORY${level}-hard.tex"

    rm -f "$RESULT_FILE"
    touch "$RESULT_FILE"
    
    echo "\\\begin{tabular} {c | c}" >> $RESULT_FILE
    echo "\\hline" >> $RESULT_FILE
    echo "Function & Stack size (bytes) & Data Size (bytes)" >> $RESULT_FILE
    echo "\\\\\\\\hline" >> $RESULT_FILE

    for FUNC_DIR in $HASH_FUNC_DIRS
    do
	echo ------- cd ${BASE_DIR}/${FUNC_DIR} ------------
	cd ${BASE_DIR}/${FUNC_DIR}

	make clean #Pour forcer la remise à jour des fichiers
	make MCU="msp430fg4618" OPT_LEVEL=$level || exit 1

	FUNC_LOG_FILE="${BASE_DIR}/${FUNC_DIR}/log${level}-hard.txt"
	touch "$FUNC_LOG_FILE"
	cat /dev/null >| $LOG_FILE

	SIM_FILE="${BASE_DIR}/${FUNC_DIR}/SIM_RESULTS${level}-hard.txt"
	touch "$SIM_FILE"
	cat /dev/null >| $SIM_FILE

	elfile=`ls *.elf`
	cp $elfile main.elf


	echo -------- MEMORY ----------
	DEVFILE=`ls /dev/* | grep 'tty.TIV'`
	mspdebug -j -d $DEVFILE uif 'prog main.elf' gdb &
	msp430-gdb -x ../address.gdb >| "$FUNC_LOG_FILE"

	if test $? -ne 0
	then
	    echo "Error $?"
	    exit
	fi

	cat "$FUNC_LOG_FILE" >> "$LOG_FILE"

	NOT_USED=`cat "$FUNC_LOG_FILE" | grep "0xffff" | cut -d '<' -f2 | cut -d '>' -f1 | cut -d ' ' -f2`
	echo $NOT_USED
	STACK_SIZE=`expr 4000 - $NOT_USED`
	DATA_SIZE=`msp430-size main.elf | grep -v text | cut -f 1`

	echo "$FUNC_DIR & $STACK_SIZE & $DATA_SIZE" >> $RESULT_FILE
	echo "\\\\\\\\\\hline" >> $RESULT_FILE

	rm $FUNC_LOG_FILE
	
	echo -------- COMPUTATION ----------
	lst_file=`ls *.lst`

	FOUND_ENC=0
	FOUND_DEC=0
	FOUND_END=0
	
	enc_file=encrypt_address.txt
	dec_file=decrypt_address.txt
	end_expe_file=end_expe_address.txt
	
	touch $enc_file $dec_file $end_expe_file
	
	cat $lst_file | while read line
	do
	    if test $FOUND_ENC -eq "1"
	    then
		echo "$line" | cut -f1 -d: > $enc_file
		FOUND_ENC=2
	    fi
	    
	    if test $FOUND_DEC -eq "1"
	    then
		echo "$line" | cut -f1 -d: > $dec_file
		FOUND_DEC=2
	    fi
	    
	    if test $FOUND_END -eq "1"
	    then
		echo "$line" | cut -f1 -d: > $end_expe_file
		FOUND_END=2
	    fi
	    
	    if [ $FOUND_ENC -eq "0" ] &&  test `echo $line | grep START_ENCRYPT`
	    then
		FOUND_ENC=1
	    fi
	    
	    if [ $FOUND_DEC -eq "0" ] &&  test `echo $line | grep START_DECRYPT`
	    then
		FOUND_DEC=1
	    fi
	    
	    if [ $FOUND_END -eq "0" ] && test `echo $line | grep END_EXPE`
	    then
		FOUND_END=1
	    fi
	done
	
	START_ENCRYPT_ADDRESS=0x`cat ./encrypt_address.txt`
	START_DECRYPT_ADDRESS=0x`cat ./decrypt_address.txt`
	END_EXPE_ADDRESS=0x`cat ./end_expe_address.txt`
	
	cp "$UTILS_DIR"/rtc_generic.gdb ./rtc.gdb
	sed -i -e "s/START_ENCRYPT_ADD/${START_ENCRYPT_ADDRESS}/g" ./rtc.gdb
	sed -i -e "s/START_DECRYPT_ADD/${START_DECRYPT_ADDRESS}/g" ./rtc.gdb
	sed -i -e "s/END_EXPE_ADD/${END_EXPE_ADDRESS}/g" ./rtc.gdb

	mspdebug -j -d $DEVFILE uif 'prog main.elf' gdb &
	msp430-gdb -x ../rtc.gdb >| "$SIM_FILE"

	pwd
	rm main.elf
	echo ---------- END -----------
    done

    echo "\end{tabular}" >> $RESULT_FILE
done

