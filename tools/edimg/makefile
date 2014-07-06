NASM := nasm
NASM_FLAGS := $(NASM_FLAGS) -f elf
CFLAGS := $(CFLAGS) -m32

.PHONY: all mostlyclean clean distclean

all: edimg

mostlyclean:
	-$(RM) *.o
	-$(RM) autodec/autodec.ias
	-$(RM) autodec/autodec.3as
	-$(RM) autodec/autodec.nas

clean: mostlyclean
	-$(RM) edimg

distclean: clean
	-$(RM) autodec.asm

edimg: edimg.o autodec.o
	$(CC) $(CFLAGS) edimg.o autodec.o -o edimg

edimg.o: edimg.c
	$(CC) $(CFLAGS) -c edimg.c -o edimg.o

autodec.o: autodec.asm
	nasm $(NASM_FLAGS) autodec.asm -o autodec.o

autodec.asm: autodec/autodec.ask
	cpp -P autodec/autodec.ask > autodec/autodec.ias
	autodec/aska autodec/autodec.ias autodec/autodec.3as
	autodec/naskcnv0 autodec/autodec.3as autodec/autodec.nas
	grep -v -E "(FORMAT|INSTRSET|OPTIMIZE|OPTION|FILE)" autodec/autodec.nas \
		| sed "s/\([D-F,S]S\):\[/\[\1:/g" \
		> autodec.asm
