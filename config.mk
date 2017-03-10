CFLAGS += -DDEBUG `pkg-config --cflags libcrypto`
LDFLAGS += `pkg-config --libs libcrypto`
CC = clang
