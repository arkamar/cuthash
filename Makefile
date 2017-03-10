include config.mk

all: hashit

hashit: hashit.o

%: %.o
	$(CC) $(LDFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) hashit *.o
