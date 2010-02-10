SOURCE=../atmega

include $(SOURCE)/Definitions.mk

.PHONY: all
all: xplain

######################

TARGET     := xplain
MCU_TARGET := atxmega128a1
include $(SOURCE)/TargetRules.mk

$(HEX): CFLAGS    += '-DNUM_LED_FLASHES=5' -DBAUD_RATE=9600
$(HEX): AVR_FREQ   = 32000000L
$(HEX): BOOT_ADDR  = 0x20000

$(ISP): FUSE0 = 
$(ISP): FUSE1 = 
$(ISP): FUSE2 = 
$(ISP): FUSE4 = 
$(ISP): FUSE5 = 

######################

include $(SOURCE)/Rules.mk
