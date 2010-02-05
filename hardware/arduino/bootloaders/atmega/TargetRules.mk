HEX      := $(PROGRAM)_$(TARGET).hex
ELF      := build/$(PROGRAM)_$(TARGET).elf
LST      := build/$(PROGRAM)_$(TARGET).lst
ISP      := $(TARGET)_isp

OBJS     := $(SOURCES:%.c=build/%_$(TARGET).o)
OBJ_DEPS := $(OBJS:.o=.d)

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

build/%_$(TARGET).o: %.c $(MAKEFILE_LIST)
	$(CC) -c $(CFLAGS) -o $@ $<

build/%_$(TARGET).s: %.c $(MAKEFILE_LIST)
	$(CC) -S $(CFLAGS) -o $@ $<

build/%_$(TARGET).d: %.c $(MAKEFILE_LIST)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:.d=.o) $(@:.d=.s) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
    include $(OBJ_DEPS)
endif
