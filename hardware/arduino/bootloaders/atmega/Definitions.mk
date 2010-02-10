# Make definitions for ATmegaBOOT
# E.Lins, 18.7.2005
# GorillaCoder 2010-02-09

ifndef SOURCE
SOURCE=.
endif

ifndef BUILD
BUILD=build
endif

# program name should not be changed...
PROGRAM    := ATmegaBOOT

# enter the parameters for the avrdude isp tool
ISPTOOL	   := stk500v2
ISPPORT	   := usb
ISPSPEED   := -b 115200

LDSECTION   = --section-start=.text=$(BOOT_ADDR)
BOOT_ADDR   = 0x3800

AVR_FREQ    = 16000000L 

# the efuse should really be 0xf8; since, however, only the lower
# three bits of that byte are used on the atmega168, avrdude gets
# confused if you specify 1's for the higher bits, see:
# http://tinker.it/now/2007/02/24/the-tale-of-avrdude-atmega168-and-extended-bits-fuses/
#
# similarly, the lock bits should be 0xff instead of 0x3f (to
# unlock the bootloader section) and 0xcf instead of 0x0f (to
# lock it), but since the high two bits of the lock byte are
# unused, avrdude would get confused.

ISPFUSES    = avrdude -c $(ISPTOOL) -p $(MCU_TARGET) -P $(ISPPORT) $(ISPSPEED) \
-e -u -U lock:w:0x3f:m -U efuse:w:0x$(EFUSE):m -U hfuse:w:0x$(HFUSE):m -U lfuse:w:0x$(LFUSE):m
ISPFLASH    = avrdude -c $(ISPTOOL) -p $(MCU_TARGET) -P $(ISPPORT) $(ISPSPEED) \
-U flash:w:$(PROGRAM)_$(TARGET).hex -U lock:w:0x0f:m

STK500   = "C:\Program Files\Atmel\AVR Tools\STK500\Stk500.exe"
STK500-1 = $(STK500) -e -d$(MCU_TARGET) -pf -vf -if$(PROGRAM)_$(TARGET).hex \
-lFF -LFF -f$(HFUSE)$(LFUSE) -EF8 -ms -q -cUSB -I200kHz -s -wt
STK500-2 = $(STK500) -d$(MCU_TARGET) -ms -q -lCF -LCF -cUSB -I200kHz -s -wt

OPTIMIZE   := -Os

DEFS       = 
LIBS       =

CC         := avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS   = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -DF_CPU=$(AVR_FREQ) \
                    -DTARGET=$(TARGET) $(DEFS)
override LDFLAGS  = -Wl,$(LDSECTION)
#override LDFLAGS = -Wl,-Map,$(PROGRAM).map,$(LDSECTION)

OBJCOPY        := avr-objcopy
OBJDUMP        := avr-objdump

SOURCES = $(PROGRAM).c spm.c
