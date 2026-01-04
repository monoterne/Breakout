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

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    Mix_Chunk* sound_select = Mix_LoadWAV("SELECT.wav");
    Mix_Chunk* sound_destroy = Mix_LoadWAV("DESTROY.wav");
    Mix_Chunk* sound_win = Mix_LoadWAV("WIN.wav");
    Mix_Chunk* sound_lose = Mix_LoadWAV("LOSE.wav");
    Mix_Music* sound_menu = Mix_LoadMUS("MENU.wav");

    if (!sound_select || !sound_destroy || !sound_win || !sound_lose || !sound_menu) {
        fprintf(stderr, "Audio Load Error: %s\n", Mix_GetError());
    }

    TTF_Font* font_big = TTF_OpenFont("PrStart.ttf", 32);
    TTF_Font* font_small = TTF_OpenFont("PrStart.ttf", 16);
    if (!font_big) {
        fprintf(stderr, "TTF_OpenFont Error: %s\n", TTF_GetError());
    }
    if (!font_small) {
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

    SDL_Texture* ball_texture = IMG_LoadTexture(renderer, "ball.png");

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
                    Mix_PlayChannel(-1, sound_select, 0);

                    if (key == SDLK_LEFT || key == SDLK_a) {
                        choosed_level--;
                        if (choosed_level < 0) {
                            choosed_level = LEVELS - 1;
                        }
                    }
                    if (key == SDLK_RIGHT || key == SDLK_d) {
                        choosed_level++;
                        if (choosed_level >= LEVELS) {
                            choosed_level = 0;
                        }
                    }
                    if (key == SDLK_SPACE || key == SDLK_RETURN) {
                        Mix_PlayChannel(-1, sound_select, 0);
                        reset();
                        game_state = PLAY;
                    }
                }
                if (game_state == LOSE || game_state == WIN) {
                    Mix_PlayMusic(sound_menu, -1); // start music again
                    if (key == SDLK_SPACE || key == SDLK_RETURN) {
                        game_state = MENU;
                    }
                }
            }
        }

        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        if (game_state == PLAY) {
            Mix_HaltMusic();
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
                score -= 50;
                if (score < 0) {
                    score = 0;
                }
                Mix_PlayChannel(-1, sound_lose, 0);
                game_state = LOSE;
            }

            if (SDL_HasIntersection(&ball, &platform)) {
                ball_move_y = -ball_move_y;
                ball.y = platform.y - ball.h;
            }

            int brick_active = 0;
            for (int i = 0; i < brick_count; i++) {
                if (brick[i].w > 0) {
                    brick_active++;
                    if (SDL_HasIntersection(&ball, &brick[i])) {
                        Mix_PlayChannel(-1, sound_destroy, 0);
                        brick[i].w = 0; // destroy brick by setting width to 0
                        ball_move_y = -ball_move_y;
                        score += 5;
                        brick_active--;
                        break; // to not destroy several bricks in one hit
                    }
                }
            }

            if (brick_active == 0) {
                Mix_PlayChannel(-1, sound_win, 0);
                game_state = WIN;
            }
        }


        SDL_SetRenderDrawColor(renderer, 15, 25, 25, 255); // Nastavení barvy na černou
        SDL_RenderClear(renderer);                      // Vykreslení pozadí

        if (game_state == MENU) {
            SDL_SetRenderDrawColor(renderer, dark_green.r, dark_green.g, dark_green.b, 255);
            SDL_RenderClear(renderer);

            int btn_size = 120;
            int btn_w = (btn_size * LEVELS) + (SPACE_Y * (LEVELS - 1));
            int btn_x = (SCREEN_WIDTH - btn_w) / 2;
            int btn_y = SCREEN_HEIGHT / 2 - 40;

            render_text(renderer, font_big, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 140, "Breakout", beige, 1);

            for (int i = 0; i < LEVELS; i++) {
                SDL_Rect btn = {
                    btn_x + i * (btn_size + SPACE_Y),
                    btn_y,
                    btn_size,
                    btn_size
                };

                if (i == choosed_level) {
                    SDL_SetRenderDrawColor(renderer, 138, 220, 143, 255);
                    SDL_Rect border = {
                        btn.x - 4,
                        btn.y - 4,
                        btn.w + 8,
                        btn.h + 8
                    };
                    SDL_RenderFillRect(renderer, &border);
                }
                SDL_SetRenderDrawColor(renderer, 15, 25, 25, 255);
                SDL_RenderFillRect(renderer, &btn);

                render_text(renderer, font_big, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 140, "Controls", beige, 1);
                render_text(renderer, font_small, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 180, "A/D or </> for go left & right", white, 1);
                render_text(renderer, font_small, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 220, "ENTER to choose the level", white, 1);


                char lvl[20];
                sprintf(lvl, "Level %d", i + 1);
                render_text(renderer, font_small, btn.x + btn_size/2, btn.y + btn_size/2, lvl, white, 1);
            }
        }

        if (game_state == PLAY) {
            SDL_SetRenderDrawColor(renderer, dark_green.r, dark_green.g, dark_green.b, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, light_green.r, light_green.g, light_green.b, 255);
            SDL_RenderFillRect(renderer, &platform);

            if (ball_texture) {
                SDL_RenderCopy(renderer, ball_texture, NULL, &ball);
            } else {
                SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, 255);
                SDL_RenderFillRect(renderer, &ball);
            }

            SDL_SetRenderDrawColor(renderer, beige.r, beige.g, beige.b, 255);
            for (int i = 0; i < brick_count; i++) {
                if (brick[i].w > 0) {
                    SDL_RenderFillRect(renderer, &brick[i]);
                }
            }
            sprintf(score_numbers, "%d", score);
            render_text(renderer, font_small, SCREEN_WIDTH - 100, 20, score_numbers, white, 1);

        }

        if (game_state == LOSE) {
            SDL_SetRenderDrawColor(renderer, dark_green.r, dark_green.g, dark_green.b, 255);
            SDL_RenderClear(renderer);

            render_text(renderer, font_big, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 60, "Game Over!", beige, 1);

            sprintf(score_numbers, "Score: %d", score);
            render_text(renderer, font_big, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, score_numbers, white, 1);

            render_text(renderer, font_small, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 60, "Press SPACE to go to menu", light_green, 1);
        }

        if (game_state == WIN) {
            SDL_SetRenderDrawColor(renderer, dark_green.r, dark_green.g, dark_green.b, 255);
            SDL_RenderClear(renderer);

            render_text(renderer, font_big, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 60, "You Won!", beige, 1);

            sprintf(score_numbers, "Score: %d", score);
            render_text(renderer, font_big, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, score_numbers, white, 1);

            render_text(renderer, font_small, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 60, "Press SPACE to go to menu", light_green, 1);
        }

        SDL_RenderPresent(renderer);  // Prezentace kreslítka

        // could try to do delta etc. (look skripta)
        //SDL_Delay(1000/60); // 60 fps
    }

    if (font_big) {
        TTF_CloseFont(font_big);
    }
    if (font_small) {
        TTF_CloseFont(font_small);
    }
    if (ball_texture) {
        SDL_DestroyTexture(ball_texture);
    }

    Mix_FreeChunk(sound_select);
    Mix_FreeChunk(sound_destroy);
    Mix_FreeChunk(sound_win);
    Mix_FreeChunk(sound_lose);
    Mix_FreeMusic(sound_menu);

    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}