#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "game.h"

#include "defines.h"

int main(int argc, char** argv) {
    InitializeGame();

    while(GameIsRunning()) {
        UpdateGame();
        RenderGame();
    }

    ShutdownGame();
    return 0;
}