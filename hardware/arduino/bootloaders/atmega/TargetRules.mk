HEX      := $(PROGRAM)_$(TARGET).hex
ELF      := build/$(PROGRAM)_$(TARGET).elf
ISP      := $(TARGET)_isp

PROG_OBJ := build/$(PROGRAM)_$(TARGET).o
SPM_OBJ  := build/spm_$(TARGET).o

OBJS     := $(SOURCES:%.c=build/%_$(TARGET).o)

OBJ_DEPS := $(OBJS:.o=.d)

PROG_S   := build/$(PROGRAM)_$(TARGET).s
SPM_S    := build/spm_$(TARGET).s

$(HEX): $(ELF)
	$(ELF2HEX_CMD)
	echo

$(ELF): $(PROG_OBJ) $(SPM_OBJ)
	$(LOAD_CMD)

$(PROG_OBJ): $(MAKEFILE_LIST)
$(PROG_OBJ): $(PROG_S)

$(SPM_OBJ): $(MAKEFILE_LIST)
$(SPM_OBJ): $(SPM_S)

$(TARGET): TARGET := $(TARGET)
$(TARGET): $(HEX)

$(ISP): $(TARGET)
$(ISP): TARGET := $(TARGET)
$(ISP): isp

build/%_$(TARGET).o: build/%_$(TARGET).s
build/%_$(TARGET).o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

build/%_$(TARGET).s: %.c
	$(AS_CMD)

PROG_DEP = $(PROG_OBJ:.o=.d)
$(PROG_DEP): $(PROGRAM).c $(MAKEFILE_LIST)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(PROG_OBJ) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

SPM_DEP = $(SPM_OBJ:.o=.d)
$(SPM_DEP): spm.c $(MAKEFILE_LIST)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(SPM_OBJ) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
    include $(OBJ_DEPS)
endif
