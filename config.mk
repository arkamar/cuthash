CFLAGS += -O2
CFLAGS += `pkg-config --cflags libcrypto`
LDFLAGS += `pkg-config --libs libcrypto`
