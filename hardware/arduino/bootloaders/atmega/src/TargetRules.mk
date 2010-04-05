# Make target rules for ATmegaBOOT
# E.Lins, 18.7.2005
# GorillaCoder 2010-02-09

BIN      := $(HEX_FOLDER)/$(PROGRAM)_$(TARGET).bin
HEX      := $(HEX_FOLDER)/$(PROGRAM)_$(TARGET).hex
ELF      := $(BUILD_FOLDER)/$(PROGRAM)_$(TARGET).elf
LST      := $(BUILD_FOLDER)/$(PROGRAM)_$(TARGET).lst
ISP      := $(TARGET)_isp

OBJS     := $(SOURCES:%.c=$(BUILD_FOLDER)/%_$(TARGET).o)
OBJ_DEPS := $(OBJS:.o=.d)

$(BIN): MCU_TARGET   := $(MCU_TARGET)
$(BIN): CFLAGS_LOCAL := $(CFLAGS_LOCAL)
$(BIN): $(OBJS:.o=.s)
$(BIN): $(ELF)
$(BIN): $(LST)
$(BIN): $(HEX_FOLDER)

$(HEX): MCU_TARGET   := $(MCU_TARGET)
$(HEX): CFLAGS_LOCAL := $(CFLAGS_LOCAL)
$(HEX): $(OBJS:.o=.s)
$(HEX): $(ELF)
$(HEX): $(LST)
$(HEX): $(HEX_FOLDER)

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TARGET): TARGET := $(TARGET)
$(TARGET): $(HEX)

$(ISP): $(TARGET)
$(ISP): MCU_TARGET := $(MCU_TARGET)
$(ISP): TARGET     := $(TARGET)
$(ISP): isp

$(BUILD_FOLDER)/%_$(TARGET).o: $(SOURCE)/%.c $(MAKEFILE_LIST) $(BUILD_FOLDER)
	$(CC) -c $(CFLAGS) $(CFLAGS_LOCAL) -o $@ $<

$(BUILD_FOLDER)/%_$(TARGET).s: $(SOURCE)/%.c $(MAKEFILE_LIST) $(BUILD_FOLDER)
	$(CC) -S $(CFLAGS) $(CFLAGS_LOCAL) -o $@ $<

$(BUILD_FOLDER)/%_$(TARGET).d: $(SOURCE)/%.c $(MAKEFILE_LIST) $(BUILD_FOLDER)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $(CFLAGS_LOCAL) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:.d=.o) $(@:.d=.s) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
#    include $(OBJ_DEPS)
endif
