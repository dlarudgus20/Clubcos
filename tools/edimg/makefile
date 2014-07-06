OUTPUT_FILE := edimg

ifneq ($(NASM), )
NASM := nasm
endif

.PHONY: all mostlyclean clean distclean

all: $(OUTPUT_FILE)

mostlyclean:
	-$(RM) *.o
	-$(RM) autodec/autodec.ias
	-$(RM) autodec/autodec.3as
	-$(RM) autodec/autodec.nas

clean: mostlyclean
	-$(RM) $(OUTPUT_FILE)

distclean: clean
	-$(RM) autodec.asm

$(OUTPUT_FILE): edimg.o autodec.o
	$(CC) $(CFLAGS) edimg.o autodec.o -o edimg

edimg.o: edimg.c
	$(CC) $(CFLAGS) -c edimg.c -o edimg.o

autodec.o: autodec.asm
	nasm -f coff autodec.asm -o autodec.o

autodec.asm: autodec/autodec.ask
	cpp -P autodec/autodec.ask > autodec/autodec.ias
	autodec/aska autodec/autodec.ias autodec/autodec.3as
	autodec/naskcnv0 autodec/autodec.3as autodec/autodec.nas
	grep -v -E "(FORMAT|INSTRSET|OPTIMIZE|OPTION|FILE)" autodec/autodec.nas \
		| sed "s/\([D-F,S]S\):\[/\[\1:/g" \
		> autodec.asm
