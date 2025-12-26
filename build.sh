
set -xe

gcc main.c -o main -Wall -Wextra -std=c99 -g -fsanitize=address -lm -Wno-unused-variable -Wuninitialized -Werror=vla `sdl2-config --cflags --libs`

./main