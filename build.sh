gcc-15 -std=c99 -g -Wall -Wextra -Wconversion -Wno-unused-result -Wdangling-pointer=1 -Werror=uninitialized -Werror=vla -Werror=return-type $(pkg-config --cflags --libs sdl3) main.c -o main

./main