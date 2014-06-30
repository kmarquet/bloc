## ##################################################
##
##   Generic Makefile 
##       common rules for all examples
##       mspgcc4 and uniarch adaptation
##
##   Contributed by Gaetan Harter 14/11/2011
##     INRIA Senslab / Senstools
##
## ##################################################


.SECONDARY:
.SUFFIXES:

# Generates all .hex file for each target contained in NAMES
#
# If there are target specific source files to compile and link,
#     define SRC_target_name variables containing the source files
#
# For required variables look down here

ifndef NAMES
$(error Required NAMES variable is undefined in Makefile.\
  Please define NAMES containing all targets name without extension)
endif # NAMES

ifndef SRC
$(error Required SRC variable is undefined in Makefile.\
  Please define SRC containing common source files that must be compiled for all targets.\
  If there is no shared sources filed, define it empty)
endif # SRC

ifndef INCLUDES
$(error Required INCLUDES variable is undefined in Makefile.\
  Please define INCLUDES containing compiler include parameter\
  If there is no include needed, define it empty)
endif # INCLUDES

CC      = msp430-gcc
LD	= msp430-gcc

OBJCOPY = msp430-objcopy
OBJDUMP = msp430-objdump
RM      = -rm -f

DEBUG   ?= -g
OPT     ?= -O3

Warnings += -Wall -Wpointer-arith -Wbad-function-cast
WARNINGS += -Wcast-align -Wsign-compare -Waggregate-return # -Wmissing-prototypes
WARNINGS += -Wunused # -Wmissing-declarations 


# GCC Uniarch compatibility

# New version on mspgcc (Uniarch) from mspgcc.sf.net defines a __MSPGCC__ variable
# http://sourceforge.net/apps/mediawiki/mspgcc/index.php?title=Devel:Uniarch#User_Visible_Changes
# CC_UNIARCH = $(shell ((echo "\#ifdef __MSPGCC__";echo "\#error";echo "\#endif")|$(CC) -o /dev/null -c -xc - 2> /dev/null)||echo "TRUE")

# ifeq ($(CC_UNIARCH), TRUE)
# GCC_4_INCLUDE = -I$(UTILS_PATH)/mspgcc-uniarch

# # unsupported with gcc-3.2.3 but usefull
# WARNINGS += -Wfatal-errors
# endif # CC_UNIARCH


ASFLAGS += -mmcu=$(MCU) -D_GNU_ASSEMBLER_
ASFLAGS += $(DEBUG) $(GCC_4_INCLUDE) $(INCLUDES)

CFLAGS +=  -mmcu=$(MCU) -DGCC_MSP430 -DREALPLATFORM=$(REALPLATFORM) -mdisable-watchdog
#CFLAGS += -MMD -MP -MF $(basename $@).d  # generate dependencies file
CFLAGS += $(DEBUG) $(GCC_4_INCLUDE) $(INCLUDES) $(WARNINGS)
CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS += -Wl,-gc-sections

#CFLAGS += -nodefaultlibs

.PHONY: all clean

TARGETS += $(addsuffix .hex, $(NAMES)) $(addsuffix .elf, $(NAMES)) $(addsuffix .lst, $(NAMES))

# sort gives us the uniqueness of object files
SRC += $(UTILS_PATH)/tools.c
SRCS = $(sort $(SRC) $(foreach name, $(NAMES), $(SRC_$(name))))

# Define all object files.
OBJ  = $(SRC:.c=.o)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

ASMOBJ = $(ASM:.S=.o)

TMP_FILES = finished.txt

all : $(TARGETS)

%.hex : %.elf
	$(OBJCOPY) -O ihex $< $@

%.lst : %.elf
	$(OBJDUMP) -dSt $< > $@


# Each binary requires ALL oject files dependency,
#     I didn't manage to get the content of SRC_target_name variable in prerequisites
# but is really linked with only the needed files

%.elf : $(OBJS) $(ASMOBJ)
	$(LD) -mdisable-watchdog -mmcu=$(MCU) -DREALPLATFORM=$(REALPLATFORM) -o $@ $(LDFLAGS) $(OBJS) $(ASMOBJ)

$(OBJS) :%.o : %.c $(filter-out %.d, $(MAKEFILE_LIST))
	$(CC) -c $(OPT_LEVEL) $(CFLAGS) $< -o $@

$(ASMOBJ) :%.o : %.S
	$(CC) -c $(OPT_LEVEL) $(ASFLAGS) $< -o $@

clean :
	$(RM) $(TARGETS) $(OBJS) ${ASMOBJ} $(DEPS) $(TMP_FILES) *.elf *.hex wsim.*

realclean: clean 
	$(RM) *.log *.trc *.vcd *.pkt

-include $(DEPS)


