#include "defines.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }

    font = TTF_OpenFont("Alegreya-Regular.ttf", 32);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont Error: %s\n", TTF_GetError());
    }

    SDL_Window* window = SDL_CreateWindow(
        "Breakout /w SDL2",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Event event;

    // game loop
    int running = 1;
    while (running == 1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;

                if (game_state == MENU) {
                    if (key == SDLK_LEFT || key == SDL_SCANCODE_A) {
                        choosed_level--;
                        if (choosed_level < 0) {
                            choosed_level = LEVELS - 1;
                        }
                    }
                    if (key == SDLK_RIGHT || key == SDL_SCANCODE_D) {
                        choosed_level++;
                        if (choosed_level >= LEVELS) {
                            choosed_level = 0;
                        }
                    }
                    if (key == SDLK_SPACE || key == SDLK_RETURN) {
                        reset();
                        game_state = PLAY;
                    }
                }
                if (game_state == LOSE) {
                    if (key == SDLK_SPACE || key == SDLK_RETURN) {
                        game_state = MENU;
                    }
                }
            }
        }
        //

        // logic
        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        if (game_state == PLAY) {
            if (key_state[SDL_SCANCODE_A] || key_state[SDL_SCANCODE_LEFT]) {
                platform.x -= PLATFORM_SPEED;
            }

            if (key_state[SDL_SCANCODE_D] || key_state[SDL_SCANCODE_RIGHT]) {
                platform.x += PLATFORM_SPEED;
            }

            if (platform.x < 0) {
                platform.x = 0;
            }

            if (platform.x > SCREEN_WIDTH - platform.w) {
                platform.x = SCREEN_WIDTH - platform.w;
            }

            ball.x += ball_move_x;
            ball.y += ball_move_y;

            if (ball.x <= 0 || ball.x >= SCREEN_WIDTH - ball.w) {
                ball_move_x = -ball_move_x;
            }

            if (ball.y <= 0) {
                ball_move_y = -ball_move_y;
            }

            if (ball.y >= SCREEN_HEIGHT) {
                game_state = LOSE;
            }

            if (SDL_HasIntersection(&ball, &platform)) {
                ball_move_y = -ball_move_y;
                ball.y = platform.y - ball.h;
            }

            for (int i = 0; i < brick_count; i++) {
                if (brick[i].w > 0 && (SDL_HasIntersection(&ball, &brick[i]))) {
                    brick[i].w = 0; // destroy brick by setting width to 0
                    ball_move_y = -ball_move_y;
                    break; // to not destroy several bricks in one hit
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); // Nastavení barvy na černou
        SDL_RenderClear(renderer);                      // Vykreslení pozadí

        if (game_state == MENU) {
            SDL_SetRenderDrawColor(renderer, 0, 100, 0, 100);
            SDL_RenderClear(renderer);

            int btn_size = 120;
            int btn_w = (btn_size * LEVELS) + (SPACE_Y * (LEVELS - 1));
            int btn_x = (SCREEN_WIDTH - btn_w) / 2;
            int btn_y = SCREEN_HEIGHT / 2 - 40;

            for (int i = 0; i < LEVELS; i++) {
                SDL_Rect btn = {
                    btn_x + i * (btn_size + SPACE_Y),
                    btn_y,
                    btn_size,
                    btn_size
                };

                if (i == choosed_level) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_Rect border = {
                        btn.x - 4,
                        btn.y - 4,
                        btn.w + 8,
                        btn.h + 8
                    };
                    SDL_RenderFillRect(renderer, &border);
                }
                SDL_SetRenderDrawColor(renderer, level_colors[i].r, level_colors[i].g, level_colors[i].b, 255);
                SDL_RenderFillRect(renderer, &btn);

                char str[2];
                sprintf(str, "Level %d", i + 1);
                SDL_Color white = {255, 255, 255, 255};
                render_text(renderer, btn.x + btn_size/2, btn.y + btn_size/2, str, white, 1);
            }
        }

        if (game_state == PLAY) {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
            SDL_RenderFillRect(renderer, &platform);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &ball);

            SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
            for (int i = 0; i < brick_count; i++) {
                if (brick[i].w > 0) {
                    SDL_RenderFillRect(renderer, &brick[i]);
                }
            }
        }

        if (game_state == LOSE) {
            SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            SDL_Rect destroyed_ball = {
                SCREEN_WIDTH/2 - 10,
                SCREEN_HEIGHT/2 - 10,
                20,
                20
            };
            SDL_RenderFillRect(renderer, &destroyed_ball);
        }

        SDL_RenderPresent(renderer);  // Prezentace kreslítka

        // could try to do delta etc. (look skripta)
        //SDL_Delay(1000/60); // 60 fps
    }

    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}