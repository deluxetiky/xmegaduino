# Makefile for ATmegaBOOT
# E.Lins, 18.7.2005
# GorillaCoder 2010-02-09

SOURCE=src

include $(SOURCE)/Definitions.mk

SUBDIRS=atmega328 atmega328_pro8 mega diecimila ng pro8 pro16 pro20 lilypad lilypad_resonator

.PHONY: all
all:

include $(SUBDIRS:%=%/Makefile)

include $(SOURCE)/Rules.mk
