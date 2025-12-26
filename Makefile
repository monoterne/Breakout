CC = gcc

TARGET = main
SRC = main.c

CFLAGS = -Wall -Wextra -std=c99 -g -fsanitize=address -Wno-unused-variable -Wuninitialized -Werror=vla
LDFLAGS = -lm -fsanitize=address

CFLAGS += $(shell sdl2-config --cflags)
LDFLAGS += $(shell sdl2-config --libs)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)