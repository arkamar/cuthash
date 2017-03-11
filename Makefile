include config.mk

BIN = cuthash
OBJ = $(BIN:=.o)

all: $(BIN)

$(BIN): $(OBJ)

%: %.o
	$(CC) $(LIBS) -o $@ $(LDFLAGS) $<

%.o: %.c config.mk
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) hashit *.o
