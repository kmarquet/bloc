#CPU        = -mmcu=msp430f1611

# Path to utilitaries
UTILS_PATH = ../utils

# define one name for each executable to be built
NAMES      = aes

# sources files specific to 'first_target'
SRC              = aes.c aes-naive.c
INCLUDES         = -I${UTILS_PATH} -Icommon

-include ${UTILS_PATH}/mspgcc.makefile
