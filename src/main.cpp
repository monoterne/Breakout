#include "defines.h"
#include <raylib.h>
#include <stdio.h>

static const Color white = {255, 255, 255, 255};
static const Color light_green = {138, 220, 143, 255};
static const Color dark_green = {17, 44, 44, 255};
static const Color beige = {199, 177, 143, 255};
static const Color button_color = {15, 25, 25, 255};

static Rectangle platform = {0, 0, PLATFORM_WIDTH, PLATFORM_HEIGHT};
static Rectangle ball = {0, 0, BALL_SIZE, BALL_SIZE};
static int ball_move_x = BALL_SPEED;  // ->
static int ball_move_y = -BALL_SPEED; // ^
static Rectangle brick[BRICK_TOTAL];
static int brick_count = 0;
static int score = 0;

static State game_state = MENU;
static int choosed_level = 0;
static const char* level_file[LEVELS] = {"level1.txt", "level2.txt", "level3.txt"};

static Sound sound_select, sound_destroy, sound_win, sound_lose;
static Music sound_menu;

static void play_sound(Sound sound) {
    if (IsSoundValid(sound)) {
        PlaySound(sound);
    }
}

static void start_menu_music() {
    if (IsMusicValid(sound_menu)) {
        StopMusicStream(sound_menu);
        PlayMusicStream(sound_menu);
    }
}

static void add_brick(int r, int c) {
    int space_x = (SCREEN_WIDTH - (BRICK_COLS * (BRICK_WIDTH + BRICK_SPACE))) / 2;
    Rectangle rect = {
        (float)(space_x + c * (BRICK_WIDTH + BRICK_SPACE)),
        (float)(SPACE_Y + r * (BRICK_HEIGHT + BRICK_SPACE)),
        BRICK_WIDTH, BRICK_HEIGHT
    };
    if (brick_count < BRICK_TOTAL) {
        brick[brick_count++] = rect;
    }
}

static void load_level(const char* file_name) {
    brick_count = 0;
    FILE* file = fopen(file_name, "r");

    if (!file) {
        printf("No %s found. Generating default bricks grid\n", file_name);
        for (int i = 0; i < 30; i++) {
            add_brick(i / BRICK_COLS, i % BRICK_COLS);
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
                fclose(file);
                return;
            }
            if (ch == '1') {
                add_brick(r, c);
            }
        }
    }
    fclose(file);
}

static void reset() {
    score = 0;
    platform.x = (SCREEN_WIDTH - platform.width) / 2;
    platform.y = SCREEN_HEIGHT - platform.height - 50;

    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    ball_move_x = BALL_SPEED;
    ball_move_y = -BALL_SPEED;

    load_level(level_file[choosed_level]);
}

static void handle_key(int key) {
    if (game_state == MENU) {
        play_sound(sound_select);
        if (key == KEY_LEFT || key == KEY_A) {
            choosed_level--;
            if (choosed_level < 0) {
                choosed_level = LEVELS - 1;
            }
        }
        if (key == KEY_RIGHT || key == KEY_D) {
            choosed_level++;
            if (choosed_level >= LEVELS) {
                choosed_level = 0;
            }
        }
        if (key == KEY_SPACE || key == KEY_ENTER) {
            play_sound(sound_select);
            reset();
            game_state = PLAY;
            if (IsMusicValid(sound_menu)) {
                StopMusicStream(sound_menu);
            }
        }
    }
    if (game_state == LOSE || game_state == WIN) {
        start_menu_music();
        if (key == KEY_SPACE || key == KEY_ENTER) {
            game_state = MENU;
        }
    }
}

static void update_play(bool move_left, bool move_right) {
    if (move_left) {
        platform.x -= PLATFORM_SPEED;
    }
    if (move_right) {
        platform.x += PLATFORM_SPEED;
    }
    if (platform.x < 0) {
        platform.x = 0;
    }
    if (platform.x > SCREEN_WIDTH - platform.width) {
        platform.x = SCREEN_WIDTH - platform.width;
    }

    ball.x += ball_move_x;
    ball.y += ball_move_y;
    if (ball.x <= 0 || ball.x >= SCREEN_WIDTH - ball.width) {
        ball_move_x = -ball_move_x;
    }
    if (ball.y <= 0) {
        ball_move_y = -ball_move_y;
    }
    if (ball.y >= SCREEN_HEIGHT) {
        play_sound(sound_lose);
        game_state = LOSE;
    }
    if (CheckCollisionRecs(ball, platform)) {
        ball_move_y = -ball_move_y;
        ball.y = platform.y - ball.height;
    }

    // The original win check counts only the bricks visited before the first hit.
    int brick_active = 0;
    for (int i = 0; i < brick_count; i++) {
        if (brick[i].width > 0) {
            brick_active++;
            if (CheckCollisionRecs(ball, brick[i])) {
                play_sound(sound_destroy);
                brick[i].width = 0; // destroy brick by setting width to 0
                ball_move_y = -ball_move_y;
                score += 5;
                brick_active--;
                break; // to not destroy several bricks in one hit
            }
        }
    }
    if (brick_active == 0) {
        play_sound(sound_win);
        game_state = WIN;
    }
}

static void render_text(Font font, int size, int x, int y, const char* text, Color color) {
    Vector2 bounds = MeasureTextEx(font, text, (float)size, 0);
    Vector2 pos = {x - bounds.x / 2, y - bounds.y / 2};
    DrawTextEx(font, text, pos, (float)size, 0, color);
}

static void draw_menu(Font font_big, Font font_small) {
    int btn_size = 120;
    int btn_w = (btn_size * LEVELS) + (SPACE_Y * (LEVELS - 1));
    int btn_x = (SCREEN_WIDTH - btn_w) / 2;
    int btn_y = SCREEN_HEIGHT / 2 - 40;

    render_text(font_big, 32, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 140, "Breakout", beige);
    for (int i = 0; i < LEVELS; i++) {
        int x = btn_x + i * (btn_size + SPACE_Y);
        if (i == choosed_level) {
            DrawRectangle(x - 4, btn_y - 4, btn_size + 8, btn_size + 8, light_green);
        }
        DrawRectangle(x, btn_y, btn_size, btn_size, button_color);

        char lvl[20];
        snprintf(lvl, sizeof(lvl), "Level %d", i + 1);
        render_text(font_small, 16, x + btn_size / 2, btn_y + btn_size / 2, lvl, white);
    }
    render_text(font_big, 32, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 140, "Controls", beige);
    render_text(font_small, 16, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 180, "A/D or </> for go left & right", white);
    render_text(font_small, 16, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 220, "ENTER to choose the level", white);
}

static void draw_play(Texture2D ball_texture, Font font_small) {
    DrawRectangleRec(platform, light_green);
    if (IsTextureValid(ball_texture)) {
        Rectangle src = {0, 0, (float)ball_texture.width, (float)ball_texture.height};
        Vector2 origin = {0, 0};
        DrawTexturePro(ball_texture, src, ball, origin, 0, white);
    } else {
        DrawRectangleRec(ball, white);
    }
    for (int i = 0; i < brick_count; i++) {
        if (brick[i].width > 0) {
            DrawRectangleRec(brick[i], beige);
        }
    }
    char score_numbers[32];
    snprintf(score_numbers, sizeof(score_numbers), "%d", score);
    render_text(font_small, 16, SCREEN_WIDTH - 100, 20, score_numbers, white);
}

static void draw_result(Font font_big, Font font_small) {
    render_text(font_big, 32, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 60,
                game_state == WIN ? "You Won!" : "Game Over!", beige);
    char score_numbers[32];
    snprintf(score_numbers, sizeof(score_numbers), "Score: %d", score);
    render_text(font_big, 32, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, score_numbers, white);
    render_text(font_small, 16, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 60, "Press SPACE to go to menu", light_green);
}

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Breakout /w raylib");
    if (!IsWindowReady()) {
        return 1;
    }
    SetExitKey(KEY_NULL);
    ChangeDirectory(GetApplicationDirectory());

    InitAudioDevice();
    if (IsAudioDeviceReady()) {
        sound_select = LoadSound("resources/audio/SELECT.wav");
        sound_destroy = LoadSound("resources/audio/DESTROY.wav");
        sound_win = LoadSound("resources/audio/WIN.wav");
        sound_lose = LoadSound("resources/audio/LOSE.wav");
        sound_menu = LoadMusicStream("resources/audio/MENU.wav");
        sound_menu.looping = true;
        start_menu_music();
    }

    Font font_big = LoadFontEx("resources/fonts/PrStart.ttf", 32, NULL, 0);
    Font font_small = LoadFontEx("resources/fonts/PrStart.ttf", 16, NULL, 0);
    Texture2D ball_texture = LoadTexture("resources/textures/ball.png");
    Camera2D camera = {};
    camera.zoom = (float)WINDOW_WIDTH / SCREEN_WIDTH;

    // game loop
    while (!WindowShouldClose()) {
        for (int key = GetKeyPressed(); key != KEY_NULL; key = GetKeyPressed()) {
            handle_key(key);
        }
        if (game_state == PLAY) {
            update_play(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT),
                        IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT));
        }
        if (IsMusicValid(sound_menu) && IsMusicStreamPlaying(sound_menu)) {
            UpdateMusicStream(sound_menu);
        }

        BeginDrawing();
        ClearBackground(dark_green);
        BeginMode2D(camera);
        if (game_state == MENU) {
            draw_menu(font_big, font_small);
        } else if (game_state == PLAY) {
            draw_play(ball_texture, font_small);
        } else {
            draw_result(font_big, font_small);
        }
        EndMode2D();
        EndDrawing();
    }

    UnloadFont(font_big);
    UnloadFont(font_small);
    if (IsTextureValid(ball_texture)) {
        UnloadTexture(ball_texture);
    }
    if (IsAudioDeviceReady()) {
        if (IsSoundValid(sound_select)) UnloadSound(sound_select);
        if (IsSoundValid(sound_destroy)) UnloadSound(sound_destroy);
        if (IsSoundValid(sound_win)) UnloadSound(sound_win);
        if (IsSoundValid(sound_lose)) UnloadSound(sound_lose);
        if (IsMusicValid(sound_menu)) UnloadMusicStream(sound_menu);
        CloseAudioDevice();
    }
    CloseWindow();
    return 0;
}
