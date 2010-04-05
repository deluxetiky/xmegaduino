# Makefile for ATmegaBOOT
# E.Lins, 18.7.2005
# GorillaCoder 2010-02-09

DEPTH := $(DEPTH).x

MAKEFILE_STACK_$(DEPTH)        := $(MAKEFILE)
MAKEFILE_FOLDER_STACK_$(DEPTH) := $(MAKEFILE_FOLDER)

MAKEFILE        := $(lastword $(MAKEFILE_LIST))
MAKEFILE        := $(abspath $(MAKEFILE))
MAKEFILE_FOLDER := $(dir $(MAKEFILE))

ifndef SOURCE
SOURCE:=$(abspath $(MAKEFILE_FOLDER)/../src)
endif

include $(SOURCE)/Definitions.mk

.PHONY: all
all: atmega328

######################

TARGET       := atmega328
MCU_TARGET   := atmega328p
CFLAGS_LOCAL := -I$(MAKEFILE_FOLDER)
CFLAGS_LOCAL += -DNUM_LED_FLASHES=0
CFLAGS_LOCAL += -DBAUD_RATE=57600
BOOT_ADDR    := 0x7800

include $(SOURCE)/TargetRules.mk

$(ISP): HFUSE = DA
$(ISP): LFUSE = FF
$(ISP): EFUSE = 05

######################

include $(SOURCE)/Rules.mk
