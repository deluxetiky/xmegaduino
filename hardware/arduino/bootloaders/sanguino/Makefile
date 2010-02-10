SOURCE=../atmega

include $(SOURCE)/Definitions.mk

.PHONY: all
all: sanguino

######################

TARGET     := sanguino
MCU_TARGET := atmega644p
include $(SOURCE)/TargetRules.mk

$(HEX): CFLAGS   += '-DNUM_LED_FLASHES=3' -DBAUD_RATE=38400
$(HEX): BOOT_ADDR = 0xF800

$(ISP): HFUSE = 1C
$(ISP): LFUSE = FF
$(ISP): EFUSE = FD

######################

include $(SOURCE)/Rules.mk
