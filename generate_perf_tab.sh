#!/bin/sh

HASH_FUNC_DIRS="AES CLEFIA128 CLEFIA192 CLEFIA256 DESXL DIRnoekeon HIGHT IDEA INDnoekeon KATAN32 KATAN48 KATAN64 KLEIN64 KLEIN80 KLEIN96 KTANTAN32 KTANTAN48 KTANTAN64 LBlock LED128 LED128_tcalc LED128_tdur LED64 LED64_tcalc LED64_tdur MCRYPTON64 MCRYPTON96 MCRYPTON128 MIBS64 MIBS80 PRESENT_SIZE PRESENT_SPEED Piccolo128 Piccolo80 SEA SKIPJACK TWINE80 TWINE128 PRINCE SIMON64_96 SIMON64_128 SIMON96_96 SIMON96_144 SIMON128_128 SPECK64_96 SPECK64_128 SPECK96_96 SPECK96_144 SPECK128_128"

BASE_DIR=`pwd`

PERF_FILE="$BASE_DIR/PERFS.txt"
SIM_FILE=SIM_RESULTS.txt
BLOCK_SIZE_FILE=BLOCK_SIZE.txt

touch "$PERF_FILE"
cat /dev/null >| $PERF_FILE


echo "\\begin{tabular} {c | c | c | c | c | c |}" >> $PERF_FILE
echo "\\hline" >> $PERF_FILE
echo "Function & Block Size (bits) & Encryption: cycle count & Encryption : cycles/byte & Decryption: cycle Count & Decryption: cycles/byte" >> $PERF_FILE

for FUNC_DIR in $HASH_FUNC_DIRS
do
    echo ------- cd ${BASE_DIR}/${FUNC_DIR} ------------
    cd ${BASE_DIR}/${FUNC_DIR}
    OCCURENCE_SMCLK=0
    cat $SIM_FILE | (while read line
    do
	case $OCCURENCE_SMCLK in
	    0)
		grepline=`echo "$line" | grep SMCLK`
		if test "$grepline"
		then
		    START_TIME=`echo "$line" | cut -f2 -d'K' | tr -d ' '`
		    OCCURENCE_SMCLK=1
		fi
		;;
	    1)
		grepline=`echo "$line" | grep SMCLK`
		if test "$grepline"
		then
		    ENCRYPT_TIME=`echo "$line" | cut -f2 -d'K' | tr -d ' '`
		    ENCRYPT_TIME=`expr $ENCRYPT_TIME - $START_TIME`
		    OCCURENCE_SMCLK=2
		fi
		;;
	    2)
		grepline=`echo "$line" | grep SMCLK`
		if test "$grepline"
		then
		    DECRYPT_TIME=`echo "$line" | cut -f2 -d'K' | tr -d ' '`
		    DECRYPT_TIME=`expr $DECRYPT_TIME - $ENCRYPT_TIME`
		fi
		;;
	esac
    done
    
    BLOCK_SIZE=`cat $BLOCK_SIZE_FILE`
    NB_BYTES=`expr "$BLOCK_SIZE" / 8`
    ENCRYPT_PERF=`expr "$ENCRYPT_TIME" / "$NB_BYTES"`
    DECRYPT_PERF=`expr "$DECRYPT_TIME" / "$NB_BYTES" `
    echo "$FUNC_DIR & $BLOCK_SIZE & $ENCRYPT_TIME & $ENCRYPT_PERF & $DECRYPT_TIME & $DECRYPT_PERF" >> $PERF_FILE
    echo "\\\\\\\\\\hline" >> $PERF_FILE
)
done

echo "\\end{tabular}" >> $PERF_FILE


