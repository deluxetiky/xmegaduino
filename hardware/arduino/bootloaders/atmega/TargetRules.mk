HEX      := $(PROGRAM)_$(TARGET).hex

PROG_OBJ := build/$(PROGRAM)_$(TARGET).o
SPM_OBJ  := build/spm_$(TARGET).o
OBJS     := $(PROG_OBJ) $(SPM_OBJ)

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
$(PROG_OBJ): $(PROGRAM).c
	$(COMPILE_CMD)

$(SPM_OBJ): $(MAKEFILE_LIST)
$(SPM_OBJ): $(SPM_S)
$(SPM_OBJ): spm.c
	$(COMPILE_CMD)

$(PROG_S): $(PROGRAM).c $(MAKEFILE_LIST)
	$(AS_CMD)

$(SPM_S): spm.c $(MAKEFILE_LIST)
	$(AS_CMD)

$(TARGET): TARGET := $(TARGET)
$(TARGET): $(HEX)

$(ISP): $(TARGET)
$(ISP): TARGET := $(TARGET)
$(ISP): isp

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

include $(OBJ_DEPS)
