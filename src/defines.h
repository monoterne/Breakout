#ifndef BREAKOUT_DEFINES_H
#define BREAKOUT_DEFINES_H

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 960

// Original playfield coordinates, used for both drawing and collisions.
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define PLATFORM_WIDTH 120
#define PLATFORM_HEIGHT 16
#define PLATFORM_SPEED 12

#define BALL_SIZE 24
#define BALL_SPEED 6

#define BRICK_WIDTH 70
#define BRICK_HEIGHT 20
#define BRICK_SPACE 10
#define BRICK_ROWS 4
#define BRICK_COLS 9
#define BRICK_TOTAL 100

#define LEVELS 3
#define SPACE_Y 40 // space from corner of screen

enum State {
    MENU,
    PLAY,
    LOSE,
    WIN
};

#endif
