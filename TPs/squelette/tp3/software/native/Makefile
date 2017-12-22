CC=gcc
INCLUDE=-I. -I../../
CFLAGS=$(INCLUDE)

main.o: hal.h
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean realclean
clean:
	$(RM) main.o

realclean: clean
	-$(RM) *~
