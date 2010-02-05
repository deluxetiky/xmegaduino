# Makefile for ATmegaBOOT
# E.Lins, 18.7.2005
# $Id$
#
# Instructions
#
# To make bootloader .hex file:
# make diecimila
# make lilypad
# make ng
# etc...
#
# To burn bootloader .hex file:
# make diecimila_isp
# make lilypad_isp
# make ng_isp
# etc...

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

.PHONY: all
all: xplain sanguino lilypad lilypad_resonator pro8 pro16 pro20 diecimila ng \
     atmega328 atmega328_pro8 mega

######################

TARGET     := xplain
MCU_TARGET := atxmega128a1
include TargetRules.mk

$(HEX): CFLAGS    += '-DNUM_LED_FLASHES=5' -DBAUD_RATE=9600
$(HEX): AVR_FREQ   = 32000000L
$(HEX): BOOT_ADDR  = 0x20000

$(ISP): FUSE0 = 
$(ISP): FUSE1 = 
$(ISP): FUSE2 = 
$(ISP): FUSE4 = 
$(ISP): FUSE5 = 

######################

TARGET     := sanguino
MCU_TARGET := atmega644p
include TargetRules.mk

$(HEX): CFLAGS   += '-DNUM_LED_FLASHES=3' -DBAUD_RATE=38400
$(HEX): BOOT_ADDR = 0xF800

$(ISP): HFUSE = 1C
$(ISP): LFUSE = FF
$(ISP): EFUSE = FD

######################

TARGET     := lilypad
MCU_TARGET := atmega168
include TargetRules.mk

$(HEX): CFLAGS += '-DMAX_TIME_COUNT=F_CPU>>1' '-DNUM_LED_FLASHES=3'
$(HEX): AVR_FREQ = 8000000L

$(ISP): HFUSE = DD
$(ISP): LFUSE = E2
$(ISP): EFUSE = 00

######################

TARGET     := lilypad_resonator
MCU_TARGET := atmega168
include TargetRules.mk

$(HEX): CFLAGS += '-DNUM_LED_FLASHES=3'
$(HEX): AVR_FREQ = 8000000L

$(ISP): HFUSE = DD
$(ISP): LFUSE = C6
$(ISP): EFUSE = 00

######################

TARGET     := pro8
MCU_TARGET := atmega168
include TargetRules.mk

$(HEX): CFLAGS += '-DNUM_LED_FLASHES=1' '-DWATCHDOG_MODS'
$(HEX): AVR_FREQ = 8000000L

$(ISP): HFUSE = DD
$(ISP): LFUSE = C6
$(ISP): EFUSE = 00

######################

TARGET     := pro16
MCU_TARGET := atmega168
include TargetRules.mk

$(HEX): CFLAGS += '-DNUM_LED_FLASHES=1' '-DWATCHDOG_MODS'

$(ISP): HFUSE = DD
$(ISP): LFUSE = C6
$(ISP): EFUSE = 00

######################

TARGET     := pro20
MCU_TARGET := atmega168
include TargetRules.mk

$(HEX): CFLAGS += '-DNUM_LED_FLASHES=1' '-DWATCHDOG_MODS'
$(HEX): AVR_FREQ = 20000000L

$(ISP): HFUSE = DD
$(ISP): LFUSE = C6
$(ISP): EFUSE = 00

######################

TARGET     := diecimila
MCU_TARGET := atmega168
include TargetRules.mk

$(HEX): CFLAGS += '-DNUM_LED_FLASHES=1'

$(ISP): HFUSE = DD
$(ISP): LFUSE = FF
$(ISP): EFUSE = 00

######################

TARGET     := ng
MCU_TARGET := atmega168
include TargetRules.mk

$(HEX): CFLAGS += '-DMAX_TIME_COUNT=F_CPU>>1' '-DNUM_LED_FLASHES=3'

$(ISP): HFUSE = DD
$(ISP): LFUSE = FF
$(ISP): EFUSE = 00

######################

TARGET     := atmega328
MCU_TARGET := atmega328p
include TargetRules.mk

$(HEX): CFLAGS    += '-DNUM_LED_FLASHES=0' -DBAUD_RATE=57600
$(HEX): BOOT_ADDR  = 0x7800

$(ISP): HFUSE = DA
$(ISP): LFUSE = FF
$(ISP): EFUSE = 05

######################

TARGET     := atmega328_pro8
MCU_TARGET := atmega328p
include TargetRules.mk

$(HEX): CFLAGS    += '-DNUM_LED_FLASHES=1' -DBAUD_RATE=57600 -DDOUBLE_SPEED
$(HEX): AVR_FREQ   = 8000000L 
$(HEX): BOOT_ADDR  = 0x7800

$(ISP): HFUSE = DA
$(ISP): LFUSE = FF
$(ISP): EFUSE = 05

######################

TARGET     := mega
MCU_TARGET := atmega1280
include TargetRules.mk

$(HEX): CFLAGS += '-DNUM_LED_FLASHES=0' -DBAUD_RATE=57600
$(HEX): BOOT_ADDR  = 0x1F000

$(ISP): HFUSE = DA
$(ISP): LFUSE = FF
$(ISP): EFUSE = F5

######################

# TARGET is set in TargetRules.mk per target definitions
isp: $(TARGET)
	$(ISPFUSES)
	$(ISPFLASH)

isp-stk500: $(PROGRAM)_$(TARGET).hex
	$(STK500-1)
	$(STK500-2)

clean:
	rm -rf build/* *.hex *.srec
	rm -rf *.map *.sym *.lst *.bin

%.hex: build/%.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	echo

%.srec: build/%.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@
	echo

%.bin: build/%.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
	echo

build/%.lst: build/%.elf
	$(OBJDUMP) -h -S $< > $@
