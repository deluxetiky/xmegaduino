HEX      := $(PROGRAM)_$(TARGET).hex
ELF      := build/$(PROGRAM)_$(TARGET).elf
ISP      := $(TARGET)_isp

OBJS     := $(SOURCES:%.c=build/%_$(TARGET).o)
OBJ_DEPS := $(OBJS:.o=.d)

PROG_OBJ := build/$(PROGRAM)_$(TARGET).o
SPM_OBJ  := build/spm_$(TARGET).o
PROG_S   := build/$(PROGRAM)_$(TARGET).s
SPM_S    := build/spm_$(TARGET).s

$(HEX): $(ELF)
	$(ELF2HEX_CMD)
	echo

$(ELF): $(OBJS)
	$(LOAD_CMD)

$(TARGET): TARGET := $(TARGET)
$(TARGET): $(HEX)

$(ISP): $(TARGET)
$(ISP): TARGET := $(TARGET)
$(ISP): isp

build/%_$(TARGET).o: %.c build/%_$(TARGET).s $(MAKEFILE_LIST)
	$(CC) -c $(CFLAGS) -o $@ $<

build/%_$(TARGET).s: %.c $(MAKEFILE_LIST)
	$(AS_CMD)

build/%_$(TARGET).d: %.c $(MAKEFILE_LIST)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:.d=.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
    include $(OBJ_DEPS)
endif
