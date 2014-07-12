CFLAGS := -m32

.PHONY: all mostlyclean clean

all: edimg

mostlyclean:
	-$(RM) *.o

clean: mostlyclean
	-$(RM) edimg

edimg: edimg.o
	$(CC) $(CFLAGS) edimg.o -o edimg

edimg.o: edimg.c
	$(CC) $(CFLAGS) -c edimg.c -o edimg.o
