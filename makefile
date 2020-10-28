MAINSRC= projet-math.c
MAINOBJ= $(MAINSRC:.c=.o)
EXEC= $(patsubst %.c,%,$(MAINSRC))

LIBSRC= $(debug.c)
LIBOBJ= $(LIBSRC:.c=.o)
LIBH= $(LIBSRC:.c=.h)

CFLAGS=-Wall
LDFLAGS=-lm

.PHONY: clean

all: $(EXEC)

$(EXEC) : $(MAINOBJ) $(LIBOBJ)
	gcc -o $@ $^ $(LDFLAGS)

%.o : %.c %.h
	gcc -c -o $@ $<

clean:
	rm -f $(EXEC) $(MAINOBJ)

