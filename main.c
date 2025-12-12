#include <SDL3/SDL.h>

int main(int argc, char** argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return -1;
    }

    SDL_Window* window;
    window = SDL_CreateWindow("Breakout", 640, 480, SDL_WINDOW_RESIZABLE);
    SDL_Event event;

    SDL_Quit();

    return 0;
}
