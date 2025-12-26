#include <SDL2/SDL.h>

// screen
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// platform
const int PLATFORM_WIDTH = 120;
const int PLATFORM_HEIGHT = 16;
const int PLATFORM_MOVE_STEP = 5;
const int PLATFORM_SPEED = 5;

// ball
const int BALL_SIZE = 8;
const int BALL_SPEED = 4;

// bricks
const int BRICK_WIDTH = 70;
const int BRICK_HEIGHT = 20;
const int BRICK_SPACE = 10; // space around bricks

#define BRICK_ROWS 4
#define BRICK_COLS (BRICK_ROWS * 2)
#define BRICK_TOTAL (BRICK_ROWS * BRICK_COLS)
#define BRICK_GRID ((BRICK_COLS * BRICK_WIDTH) + ((BRICK_COLS - 1) * BRICK_SPACE))

int main(int agrc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Breakout /w SDL2", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    int running = 1;

    int space_y = 50; // space from corner of screen, used for platform and bricks

    SDL_Rect platform;
    platform.x = (SCREEN_WIDTH - PLATFORM_WIDTH)/2;
    platform.y = SCREEN_HEIGHT - PLATFORM_HEIGHT - space_y;
    platform.w = PLATFORM_WIDTH;
    platform.h = PLATFORM_HEIGHT;

    SDL_Rect ball;
    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    ball.w = BALL_SIZE;
    ball.h = BALL_SIZE;

    int ball_move_x = BALL_SPEED; // ->
    int ball_move_y = -BALL_SPEED; // ^

    SDL_Rect brick[BRICK_TOTAL];
    int brick_x = (SCREEN_WIDTH - BRICK_GRID)/2;
    int brick_y = space_y;
    int brick_count = 0;

    for (int r = 0; r < BRICK_ROWS; ++r) {
        for (int c = 0; c < BRICK_COLS; ++c) {
            brick[brick_count].x = brick_x;
            brick[brick_count].y = brick_y;
            brick[brick_count].w = BRICK_WIDTH;
            brick[brick_count].h = BRICK_HEIGHT;

            // move right for the next brick
            brick_x += (BRICK_WIDTH + BRICK_SPACE);

            ++brick_count;
        }

        // back to initial value
        brick_x = (SCREEN_WIDTH - BRICK_GRID)/2;

        // y down to new line of bricks grid
        brick_y += (BRICK_HEIGHT + BRICK_SPACE);
    }

    // -- event game loop
    while (running == 1)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }

        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        if (key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
            platform.x = platform.x - PLATFORM_SPEED;
            //printf("pressed A or LEFT\n");
        }

        if (key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
            platform.x = platform.x + PLATFORM_SPEED;
            //printf("pressed D or RIGHT\n");
        }

        if (platform.x < 0) {
            platform.x = 0;
        }

        if (platform.x > SCREEN_WIDTH - PLATFORM_WIDTH) {
            platform.x = SCREEN_WIDTH - PLATFORM_WIDTH;
        }

        ball.x += ball_move_x;
        ball.y += ball_move_y;

        if (ball.x <= 0) {
            ball.x = 0;
            ball_move_x = -ball_move_x; // change direction
        }

        if (ball.y <= 0) {
            ball.y = 0;
            ball_move_y = -ball_move_y;
        }

        if (ball.x >= SCREEN_WIDTH - BALL_SIZE) {
            ball.x = SCREEN_WIDTH - BALL_SIZE;
            ball_move_x = -ball_move_x;
        }

        if (ball.y >= SCREEN_HEIGHT - BALL_SIZE) {
            ball.y = SCREEN_HEIGHT - BALL_SIZE;
            ball_move_y = -ball_move_y;
        }

        if (SDL_HasIntersection(&ball, &platform)) {
            ball_move_y = -ball_move_y; // flip to not stuck on platform
            ball.y = platform.y - BALL_SIZE;
        }

        for (int i = 0; i < BRICK_TOTAL; ++i) {
            if (brick[i].x > -1) {
                if (SDL_HasIntersection(&ball, &brick[i])) {
                    brick[i].x = -1000; // -1000 so it can't be seen, like destroyed
                    ball_move_y = -ball_move_y;
                    break; // to not hit several bricks in one run
                }
            }
        }
        // background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nastavení barvy na černou
        SDL_RenderClear(renderer);                      // Vykreslení pozadí
        //

        // platform
        SDL_SetRenderDrawColor(renderer, 0, 180, 0, 255);
        SDL_RenderFillRect(renderer, &platform);
        //

        // ball
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &ball);
        //

        // bricks
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < BRICK_TOTAL; ++i) {
            if (brick[i].x > -1) {
                SDL_RenderFillRect(renderer, &brick[i]);
            }
        }
        //

        //
        SDL_RenderPresent(renderer);  // Prezentace kreslítka
        // --
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
