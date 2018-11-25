# Makefile, versao 1
# Sistemas Operativos, DEI/IST/ULisboa 2018-19

SSOURCES = CircuitRouter-AdvShell.c
SSOURCES+= shlib/chprocess.c
SSOURCES+= ../lib/commandparser.c ../lib/vector.c
SOBJS = $(SSOURCES:%.c=%.o)

CSOURCES = CircuitRouter-Client.c
COBJS = $(CSOURCES:%.c=%.o)

CC   = gcc
CFLAGS =-Wall -std=gnu99 -I../
LDFLAGS=-lm
STARGET = CircuitRouter-AdvShell
CTARGET = CircuitRouter-Client

all: $(STARGET) $(CTARGET)

$(STARGET): $(SOBJS)
	@echo target
	$(CC) $(CFLAGS) $^ -o $(STARGET) $(LDFLAGS) 

$(CTARGET): $(COBJS)
	@echo target
	$(CC) $(CFLAGS) $^ -o $(CTARGET) $(LDFLAGS) 

CircuitRouter-AdvShell.o: CircuitRouter-AdvShell.c shlib/shellprotocol.h shlib/chprocess.c shlib/chprocess.h ../lib/commandparser.c ../lib/commandparser.h ../lib/vector.c ../lib/vector.h ../lib/types.h
shlib/chprocess.o: shlib/chprocess.c shlib/chprocess.h shlib/shellprotocol.h ../lib/timer.h ../lib/types.h
../lib/vector.o: ../lib/vector.c ../lib/vector.h ../lib/types.h ../lib/utility.h
../lib/commandparser.o: ../lib/commandparser.c ../lib/commandparser.h

CircuitRouter-Client.o: CircuitRouter-Client.c ../lib/types.h shlib/shellprotocol.h

$(SOBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

$(COBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean: sclean cclean

sclean:
	@echo Cleaning shell...
	rm -f $(SOBJS) $(STARGET)	

cclean:
	@echo Cleaning client...
	rm -f $(COBJS) $(CTARGET)
