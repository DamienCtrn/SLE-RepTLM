CC=gcc
CSOURCE=$(wildcard ./../*.c)
INCLUDE=-I. -I../../
CFLAGS=$(INCLUDE)

main.o: $(CSOURCE) hal.h
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean realclean
clean:
	$(RM) main.o

realclean: clean
	-$(RM) *~
