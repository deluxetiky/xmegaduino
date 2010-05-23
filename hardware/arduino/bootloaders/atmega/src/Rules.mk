# Make rules for ATmegaBOOT
# E.Lins, 18.7.2005
# GorillaCoder 2010-02-09

ifndef RULES_MK
RULES_MK:=1

# TARGET is set in TargetRules.mk per target definitions
isp: $(TARGET)
	$(ISPFUSES)
	$(ISPFLASH)

isp-stk500: $(HEX_FOLDER)/$(PROGRAM)_$(TARGET).hex
	$(STK500-1)
	$(STK500-2)

clean:
	rm -rf $(BUILD_FOLDER)/*
	rm -rf $(HEX_FOLDER)/*.hex $(HEX_FOLDER)/*.bin

$(HEX_FOLDER)/%.hex: $(BUILD_FOLDER)/%.elf
	mkdir -p $(HEX_FOLDER)
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	echo

$(HEX_FOLDER)/%.srec: $(BUILD_FOLDER)/%.elf
	mkdir -p $(HEX_FOLDER)
	$(OBJCOPY) -j .text -j .data -O srec $< $@
	echo

$(HEX_FOLDER)/%.bin: $(BUILD_FOLDER)/%.elf
	mkdir -p $(HEX_FOLDER)
	$(OBJCOPY) -j .text -j .data -O binary $< $@
	echo

$(BUILD_FOLDER)/%.lst: $(BUILD_FOLDER)/%.elf
	$(OBJDUMP) -h -S $< > $@

$(BUILD_FOLDER):
	mkdir -p $(BUILD_FOLDER)

$(HEX_FOLDER):
	mkdir -p $(HEX_FOLDER)

endif
