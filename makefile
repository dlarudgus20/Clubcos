# Copyright (c) 2014, 임경현 (dlarudgus20)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## @file makefile
## @author dlarudgus20
## @copyright The BSD (2-Clause) License

# Clubcos makefile

# config
ifeq ($(CONFIG), Release)
	# ok
else ifeq ($(CONFIG), Debug)
	# ok
else ifeq ($(CONFIG), )
	# default to Debug
	CONFIG := Debug
else
	$(error [CONFIG] '$(CONFIG)': unknown configuration.)
endif

# directories
DIR_IMG := img/$(CONFIG)
DIR_BIN := bin/$(CONFIG)
DIR_DEP := dep/$(CONFIG)
DIR_OBJ := obj/$(CONFIG)
DIR_GEN := gen/$(CONFIG)
DIR_SRC := src
DIR_TOOLS := tools

# tools
TARGET_PLATFORM := i686-pc-elf

CC := $(TARGET_PLATFORM)-gcc
OBJCOPY := $(TARGET_PLATFORM)-objcopy
OBJDUMP := $(TARGET_PLATFORM)-objdump
NM := $(TARGET_PLATFORM)-nm
NASM := nasm
NDISASM := ndisasm
GDB := $(TARGET_PLATFORM)-gdb

EDIMG := $(DIR_TOOLS)/edimg/edimg
DIR_EXCPP := $(DIR_TOOLS)/excpp
EXCPP := $(DIR_EXCPP)/excpp

ifeq ($(TOOLS_CXX), )
TOOLS_CXX := /cygdrive/c/MinGW/bin/g++
endif
ifeq ($(TOOLS_CXXFLAGS), )
TOOLS_CXXFLAGS := -std=c++11 -O3 -Wall -static
endif

# flags
CFLAGS := $(CFLAGS) -std=c99 -m32 -ffreestanding -I$(DIR_SRC) \
	-Wall -Wextra -Wno-unused-parameter -fpack-struct
#	-mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow
LDFLAGS := $(LDFLAGS) -nostdlib -Xlinker -melf_i386
OBJDUMP_FLAGS := $(OBJDUMP_FLAGS) -M intel
NM_FLAGS := $(NM_FLAGS) --line-numbers --print-size --print-armap --numeric-sort
NASM_FLAGS := $(NASM_FLAGS) -i$(DIR_SRC)/

ifeq ($(CONFIG), Release)
CFLAGS := $(CFLAGS) -DNDEBUG -Ofast -flto
else
CFLAGS := $(CFLAGS) -ggdb -DDEBUG
endif

# files
ORIGIN_IMG := img/floppy_origin.img
OUTPUT_IMG := $(DIR_IMG)/floppy.img

KERNEL_SYS_FILE := $(DIR_BIN)/Clubcos.sys
KERNEL_ELF_FILE := $(DIR_BIN)/Clubcos.elf

C_SOURCES := $(wildcard $(DIR_SRC)/*.c)
C_OBJECTS := $(patsubst $(DIR_SRC)/%.c, $(DIR_OBJ)/%.c.o, $(C_SOURCES))

EXCP_SOURCES := $(wildcard $(DIR_SRC)/*.excp)
EXCP_OUTPUTS := $(patsubst $(DIR_SRC)/%.excp, $(DIR_GEN)/%.excp.c, $(EXCP_SOURCES))
EXCP_OBJECTS := $(patsubst $(DIR_GEN)/%.excp.c, $(DIR_OBJ)/%.excp.o, $(EXCP_OUTPUTS))

ASM_SOURCES := $(wildcard $(DIR_SRC)/*asm)
ASM_OBJECTS := $(patsubst $(DIR_SRC)/%.asm, $(DIR_OBJ)/%.asm.o, $(ASM_SOURCES))

DEPENDENCIES := $(patsubst $(DIR_SRC)/%.c, $(DIR_DEP)/%.c.d, $(C_SOURCES)) \
	$(patsubst $(DIR_SRC)/%.excp, $(DIR_DEP)/%.excp.d, $(EXCP_SOURCES)) \
	$(patsubst $(DIR_SRC)/%.asm, $(DIR_DEP)/%.asm.d, $(ASM_SOURCES))

LINK_SCRIPT := $(DIR_SRC)/link_script.ld

# qemu
ifeq ($(QEMU), )
QEMU := qemu-system-i386w
endif
QEMU_FLAGS := -L . -m 64 -fda $(OUTPUT_IMG) -boot a -localtime -M pc
QEMU_DEBUG_FLAGS := $(QEMU_FLAGS) -gdb tcp:127.0.0.1:1234 -S

# bochs
ifeq ($(QEMU), )
BOCHSDBG := bochsdbg
endif
BOCHSRC := bochsrc_$(CONFIG).bxrc

# phony targets
.PHONY: all make_folder rebuild run rerun run_dbg dbg dbg_vm run_bochs \
		distclean clean mostlyclean tools tools_clean

all: make_folder $(EXCPP) $(OUTPUT_IMG)

make_folder:
	mkdir -p img
	mkdir -p $(DIR_IMG)
	mkdir -p bin
	mkdir -p $(DIR_BIN)
	mkdir -p dep
	mkdir -p $(DIR_DEP)
	mkdir -p obj
	mkdir -p $(DIR_OBJ)
	mkdir -p gen
	mkdir -p $(DIR_GEN)

rebuild:
	make clean
	make

run:
	make all
	$(QEMU) $(QEMU_FLAGS)

rerun:
	make clean
	make run

run_dbg:
	make all
	$(QEMU) $(QEMU_DEBUG_FLAGS)

dbg:
	$(GDB) $(KERNEL_ELF_FILE) "-ex=target remote :1234"

dbg_vm:
	$(GDB) $(KERNEL_ELF_FILE) "-ex=target remote :8832"

run_bochs:
	make all
	-$(BOCHSDBG) -qf $(BOCHSRC)

distclean: clean tools_clean

clean: mostlyclean
	-$(RM) $(DIR_BIN)/*
	-$(RM) $(OUTPUT_IMG)
	-$(RM) $(DIR_DEP)/*

mostlyclean:
	-$(RM) $(DIR_OBJ)/*
	-$(RM) $(DIR_GEN)/*

tools: $(EXCPP)

tools_clean:
	-$(RM) $(EXCPP)

# rules
$(OUTPUT_IMG): $(KERNEL_SYS_FILE)
	$(EDIMG) imgin:$(ORIGIN_IMG) \
		copy from:$(KERNEL_SYS_FILE) to:@: \
		imgout:$@

$(KERNEL_SYS_FILE): $(C_OBJECTS) $(ASM_OBJECTS) $(EXCP_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -T $(LINK_SCRIPT) -o $(KERNEL_ELF_FILE) $^ \
		 -Xlinker -Map=$(DIR_OBJ)/Clubcos_elf.map
	$(NM) $(NM_FLAGS) $(KERNEL_ELF_FILE) > $(DIR_OBJ)/Clubcos_elf.nm
	$(NDISASM) -b 32 $(KERNEL_ELF_FILE) > $(DIR_OBJ)/Clubcos_elf.disasm

	$(OBJCOPY) -j .text -j .data -j .rodata -j .bss -S -g $(KERNEL_ELF_FILE) $(KERNEL_SYS_FILE)
	$(NM) $(NM_FLAGS) $(KERNEL_SYS_FILE) > $(DIR_OBJ)/Clubcos_sys.nm
	$(NDISASM) -b 32 $(KERNEL_SYS_FILE) > $(DIR_OBJ)/Clubcos_sys.disasm

	$(NM) --line-numbers --numeric-sort $(KERNEL_SYS_FILE) \
		| sed '/.*/ s/\(.*\) . \(.*\)/\1 \2/g' > $(DIR_OBJ)/Clubcos.sym

$(DIR_OBJ)/%.c.o: $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(OBJDUMP) $(OBJDUMP_FLAGS) -D $@ > $(DIR_OBJ)/$*.c.dump
$(DIR_DEP)/%.c.d: $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) $< -MM -MT $(DIR_OBJ)/$*.c.o \
		| sed 's@\($(DIR_OBJ)/$*.c.o\)[ :]*@\1 $@ : @g' > $@

$(DIR_GEN)/%.excp.c: $(DIR_SRC)/%.excp
	$(EXCPP) $< $@

$(DIR_OBJ)/%.excp.o: $(DIR_GEN)/%.excp.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(OBJDUMP) $(OBJDUMP_FLAGS) -D $@ > $(DIR_OBJ)/$*.excp.dump
$(DIR_DEP)/%.excp.d: $(DIR_GEN)/%.excp.c
	$(CC) $(CFLAGS) $< -MM -MT $(DIR_OBJ)/$*.excp.o \
		| sed 's@\($(DIR_OBJ)/$*.excp.o\)[ :]*@\1 $@ : @g' > $@

$(DIR_OBJ)/%.asm.o: $(DIR_SRC)/%.asm
	$(NASM) $(NASM_FLAGS) -f elf $< -l $(DIR_OBJ)/$*.lst -o $@
$(DIR_DEP)/%.asm.d: $(DIR_SRC)/%.asm
	$(NASM) $(NASM_FLAGS) $< -M -MT $(DIR_OBJ)/$*.asm.o \
		| sed 's@\($(DIR_OBJ)/$*.asm.o\)[ :]*@\1 $@ : @g' > $@

# include dependencies
ifneq ($(MAKECMDGOALS), distclean)
ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), mostlyclean)
ifneq ($(MAKECMDGOALS), tools)
ifneq ($(MAKECMDGOALS), tools_clean)
include $(DEPENDENCIES)
endif
endif
endif
endif
endif

# tools build
$(EXCPP): $(wildcard $(DIR_EXCPP)/src/*.cpp)
	$(TOOLS_CXX) $(TOOLS_CXXFLAGS) $^ -o $@
