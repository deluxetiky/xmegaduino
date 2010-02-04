HEX      := $(PROGRAM)_$(TARGET).hex

PROG_OBJ := build/$(PROGRAM)_$(TARGET).o
SPM_OBJ  := build/spm_$(TARGET).o

PROG_S   := build/$(PROGRAM)_$(TARGET).s
SPM_S    := build/spm_$(TARGET).s

$(TARGET): TARGET := $(TARGET)
$(TARGET): $(HEX)
$(HEX): $(ELF)
	$(ELF2HEX_CMD)
	echo
$(ELF): $(PROG_OBJ) $(SPM_OBJ)
	$(LOAD_CMD)
$(PROG_OBJ): $(PROGRAM).c $(MAKEFILE_LIST)
	$(COMPILE_CMD)
$(SPM_OBJ): spm.c $(MAKEFILE_LIST)
	$(COMPILE_CMD)
#$(TARGET): $(PROG_S)
#$(TARGET): $(SPM_S)

$(PROG_S): $(PROGRAM).c
	$(AS_CMD)
$(SPM_S): spm.c
	$(AS_CMD)

$(ISP): $(TARGET)
$(ISP): TARGET := $(TARGET)
$(ISP): isp
