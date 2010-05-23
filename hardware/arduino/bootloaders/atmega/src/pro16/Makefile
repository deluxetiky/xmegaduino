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
all: pro16

######################

TARGET       := pro16
MCU_TARGET   := atmega168
CFLAGS_LOCAL := -I$(MAKEFILE_FOLDER)
CFLAGS_LOCAL += -DNUM_LED_FLASHES=3
CFLAGS_LOCAL += -DWATCHDOG_MODS
CFLAGS_LOCAL += -DBAUD_RATE=19200

include $(SOURCE)/TargetRules.mk

$(ISP): HFUSE = DD
$(ISP): LFUSE = C6
$(ISP): EFUSE = 00

######################

include $(SOURCE)/Rules.mk

MAKEFILE        := $(MAKEFILE_STACK_$(DEPTH))
MAKEFILE_FOLDER := $(MAKEFILE_FOLDER_STACK_$(DEPTH))
DEPTH           := $(basename $(DEPTH))
