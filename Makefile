CC = gcc
TARGET = src/*.c

CFLAGS = -Wall -Wextra -Werror=vla -std=c99
LFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

TARGET_NAME = Breakout

all : $(TARGET)
	$(CC) $(TARGET) $(CFLAGS) $(LFLAGS) -o $(TARGET_NAME)
