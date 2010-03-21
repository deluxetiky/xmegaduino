# Make target rules for ATmegaBOOT
# E.Lins, 18.7.2005
# GorillaCoder 2010-02-09

BIN      := $(PROGRAM)_$(TARGET).bin
HEX      := $(PROGRAM)_$(TARGET).hex
ELF      := $(BUILD)/$(PROGRAM)_$(TARGET).elf
LST      := $(BUILD)/$(PROGRAM)_$(TARGET).lst
ISP      := $(TARGET)_isp

OBJS     := $(SOURCES:%.c=$(BUILD)/%_$(TARGET).o)
OBJ_DEPS := $(OBJS:.o=.d)

$(BIN): MCU_TARGET := $(MCU_TARGET)
$(BIN): $(ELF) $(OBJS:.o=.s)
$(BIN): $(LST)

$(HEX): MCU_TARGET := $(MCU_TARGET)
$(HEX): $(ELF) $(OBJS:.o=.s)
$(HEX): $(LST)

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TARGET): TARGET := $(TARGET)
$(TARGET): $(HEX)

$(ISP): $(TARGET)
$(ISP): MCU_TARGET := $(MCU_TARGET)
$(ISP): TARGET     := $(TARGET)
$(ISP): isp

$(BUILD)/%_$(TARGET).o: $(SOURCE)/%.c $(MAKEFILE_LIST)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/%_$(TARGET).s: $(SOURCE)/%.c $(MAKEFILE_LIST)
	$(CC) -S $(CFLAGS) -o $@ $<

$(BUILD)/%_$(TARGET).d: $(SOURCE)/%.c $(MAKEFILE_LIST) $(BUILD)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:.d=.o) $(@:.d=.s) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BUILD):
	mkdir -p $(BUILD)

ifneq ($(MAKECMDGOALS),clean)
    include $(OBJ_DEPS)
endif
