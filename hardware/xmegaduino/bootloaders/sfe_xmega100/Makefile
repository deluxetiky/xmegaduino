TOP:=../../..

ifndef ARDUINO_BOOTLOADER_FOLDER
ARDUINO_BOOTLOADER_FOLDER:=$(TOP)/arduino/bootloaders
endif

SOURCE:=$(ARDUINO_BOOTLOADER_FOLDER)/atmega

include $(SOURCE)/Definitions.mk

.PHONY: all
all: xplain

######################

TARGET     := sfe_xmega100
MCU_TARGET := atxmega128a1
include $(SOURCE)/TargetRules.mk

$(TARGET): $(BIN)

$(HEX): CFLAGS    += '-DNUM_LED_FLASHES=3'
$(HEX): AVR_FREQ   = 32000000L
$(HEX): BOOT_ADDR  = 0x20000

$(BIN): CFLAGS    += '-DNUM_LED_FLASHES=3'
$(BIN): AVR_FREQ   = 32000000L
$(BIN): BOOT_ADDR  = 0x20000

$(ISP): FUSE0 = 
$(ISP): FUSE1 = 
$(ISP): FUSE2 = 
$(ISP): FUSE4 = 
$(ISP): FUSE5 = 

######################

include $(SOURCE)/Rules.mk
