# Make rules for ATmegaBOOT
# E.Lins, 18.7.2005
# GorillaCoder 2010-02-09

# TARGET is set in TargetRules.mk per target definitions
isp: $(TARGET)
	$(ISPFUSES)
	$(ISPFLASH)

isp-stk500: $(PROGRAM)_$(TARGET).hex
	$(STK500-1)
	$(STK500-2)

clean:
	rm -rf $(BUILD)/* *.hex *.srec *.bin
	rm -rf *.map *.sym *.lst *.bin

%.hex: $(BUILD)/%.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	echo

%.srec: $(BUILD)/%.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@
	echo

%.bin: $(BUILD)/%.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
	echo

$(BUILD)/%.lst: $(BUILD)/%.elf
	$(OBJDUMP) -h -S $< > $@
