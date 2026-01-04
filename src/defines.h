#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define PLATFORM_WIDTH 120
#define PLATFORM_HEIGHT 16
#define PLATFORM_MOVE_STEP 4
#define PLATFORM_SPEED 12

#define BALL_SIZE 24
#define BALL_SPEED 6

#define BRICK_WIDTH 70
#define BRICK_HEIGHT 20
#define BRICK_SPACE 10
#define BRICK_ROWS 4
#define BRICK_COLS 9
#define BRICK_TOTAL 100 // do smth with bricks

#define LEVELS 3

#define SPACE_Y 40 // space from corner of screen

int score = 0;
char score_numbers[200];

SDL_Color white = {255, 255, 255, 255};
SDL_Color light_green = {138, 220, 143, 255};
SDL_Color dark_green = {17, 44, 44, 255};
SDL_Color beige = {199, 177, 143, 255};



SDL_Rect platform = {
    (SCREEN_WIDTH - PLATFORM_WIDTH) / 2,
    (SCREEN_HEIGHT - PLATFORM_HEIGHT - SPACE_Y),
    PLATFORM_WIDTH,
    PLATFORM_HEIGHT
};

SDL_Rect ball = {
    (SCREEN_WIDTH / 2),
    (SCREEN_HEIGHT / 2),
    BALL_SIZE,
    BALL_SIZE
};

int ball_move_x = BALL_SPEED; // ->
int ball_move_y = -BALL_SPEED; // ^

SDL_Rect brick[BRICK_TOTAL];
int brick_count = 0;

TTF_Font* font = NULL;

typedef enum {
    MENU,
    PLAY,
    LOSE,
    WIN,
} State;

State game_state = MENU; // maybe find some way to do it using SDL, CreatePopupWindow or smth

int choosed_level = 0;  
const char* level_file[LEVELS] = {
    "level1.txt",
    "level2.txt",
    "level3.txt"
};

void render_text(SDL_Renderer* renderer, TTF_Font* font, int x, int y, const char* text, SDL_Color color, int center) {
    if (!font) {
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Blended(
        font,
        text,
        color
    );
    if (!surface) {
        printf("Text Render Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(
        renderer,
        surface
    );

    SDL_Rect text_rect = {
        x,
        y,
        surface->w,
        surface->h
    };

    if (center) {
        text_rect.x = x-(surface->w / 2);
        text_rect.y = y-(surface->h / 2);
    }

    SDL_RenderCopy(renderer, texture, NULL, &text_rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void load_level(const char* file_name) {
    brick_count = 0;
    FILE* file = fopen(file_name, "r");

    int space_x = (SCREEN_WIDTH - (BRICK_COLS * (BRICK_WIDTH + BRICK_SPACE))) / 2;

    if (!file) {
        printf("No %s found. Generating default bricks grid\n", file_name);
        for (int i = 0; i < 30; i++) {
            int r = i / BRICK_COLS;
            int c = i % BRICK_COLS;
            brick[brick_count++] = (SDL_Rect) {
                space_x + c*(BRICK_WIDTH + BRICK_SPACE),
                SPACE_Y + r*(BRICK_HEIGHT + BRICK_SPACE),
                BRICK_WIDTH,
                BRICK_HEIGHT
            };
        }
        return;
    }

    printf("Loaded: %s\n", file_name);
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            int ch = fgetc(file);
            while (ch == '\n' || ch == '\r' || ch == ' ') {
                ch = fgetc(file);
            }

            if (ch == EOF) {
                break;
            }

            if (ch == '1') {
                if (brick_count < BRICK_TOTAL) {
                    brick[brick_count++] = (SDL_Rect) {
                    space_x + c*(BRICK_WIDTH + BRICK_SPACE),
                    SPACE_Y + r*(BRICK_HEIGHT + BRICK_SPACE),
                    BRICK_WIDTH, BRICK_HEIGHT};
                }
            }
        }
    }   
    fclose(file);
}

void reset() {
    score = 0;
    platform.x = (SCREEN_WIDTH - platform.w) / 2;
    platform.y = SCREEN_HEIGHT - platform.h - 50;

    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    ball_move_x = BALL_SPEED;
    ball_move_y = -BALL_SPEED;

    load_level(level_file[choosed_level]);
}