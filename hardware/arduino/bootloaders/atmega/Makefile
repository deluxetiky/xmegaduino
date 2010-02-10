# Makefile for ATmegaBOOT
# E.Lins, 18.7.2005
# GorillaCoder 2010-02-09
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

include Definitions.mk

.PHONY: all
all: lilypad lilypad_resonator pro8 pro16 pro20 diecimila ng \
     atmega328 atmega328_pro8 mega

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

include Rules.mk
