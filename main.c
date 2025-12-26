#include <SDL2/SDL.h>

// screen
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//platform
const int PLATFORM_WIDTH = 120;
const int PLATFORM_HEIGHT = 16;
const int PLATFORM_MOVE_STEP = 5;
const int PLATFORM_CENTER = (SCREEN_WIDTH - PLATFORM_WIDTH)/2;

// bricks
const int BRICK_WIDTH = 70;
const int BRICK_HEIGHT = 20;
const int BRICK_SPACE = 10; // space around bricks

#define BRICK_ROWS 4
#define BRICK_COLS (BRICK_ROWS * 2)
#define TOTAL_BRICKS (BRICK_ROWS * BRICK_COLS)
#define TOTAL_BRICKS_WIDTH ((BRICK_COLS * BRICK_WIDTH) + ((BRICK_COLS - 1) * BRICK_SPACE))

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

    SDL_Rect platform;
    platform.x = PLATFORM_CENTER;
    platform.y = SCREEN_HEIGHT - PLATFORM_HEIGHT;
    platform.w = PLATFORM_WIDTH;
    platform.h = PLATFORM_HEIGHT;

    SDL_Rect brick[TOTAL_BRICKS];
    int brick_x = (SCREEN_WIDTH - TOTAL_BRICKS_WIDTH)/2;
    int brick_y = 50; // 50 from top
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
        brick_x = (SCREEN_WIDTH - TOTAL_BRICKS_WIDTH)/2;

        // y down to new line of bricks
        brick_y += (BRICK_HEIGHT + BRICK_SPACE);
    }

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

        if (key_state[SDL_SCANCODE_W] || key_state[SDL_SCANCODE_UP]) {
            platform.y = platform.y - 5;
            //printf("pressed W or UP\n");
        }
        if (key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
            platform.x = platform.x - 5;
            //printf("pressed A or LEFT\n");
        }
        if (key_state[SDL_SCANCODE_S] || key_state[SDL_SCANCODE_DOWN]) {
            platform.y = platform.y + 5;
            //printf("pressed S or DOWN\n");
        }
        if (key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
            platform.x = platform.x + 5;
            //printf("pressed D or RIGHT\n");
        }

        // background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nastavení barvy na černou
        SDL_RenderClear(renderer);                      // Vykreslení pozadí
        //

        // platform
        SDL_SetRenderDrawColor(renderer, 0, 180, 0, 255);
        SDL_RenderFillRect(renderer, &platform);
        //

        // bricks
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < TOTAL_BRICKS; ++i) {
            SDL_RenderFillRect(renderer, &brick[i]);
        }

        // --
        SDL_RenderPresent(renderer);  // Prezentace kreslítka
    }

    // --
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
