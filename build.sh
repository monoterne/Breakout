
set -xe

gcc -Wall -Wno-unused-result -Wconversion -Wno-sign-conversion -Werror=vla -Werror=uninitialized -Werror=vla -Werror=return-type -Wextra -o main -g -std=c99 main.c `sdl2-config --cflags --libs`

./main
