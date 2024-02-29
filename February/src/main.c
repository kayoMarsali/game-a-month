#include <SDL.h>
#include <SDL_ttf.h>

#include "game.h"

#include "defines.h"

int main(int argc, char** argv) {
    u64 lastFrameTime = 0;
    u64 frameStartTime = 0;
    u64 frameEndTime = 0;
    u64 frameTime = 0;
    u64 targetFPS = 60;
    u64 frameDelay = 1000 / targetFPS;
    f64 deltaTime = 0.0;


    InitializeGame();

    while(GameIsRunning()) {
        frameStartTime = SDL_GetTicks64();
        UpdateGame(deltaTime);
        RenderGame();

        frameEndTime = SDL_GetTicks64();
        frameTime = frameEndTime - frameStartTime;
        if(frameDelay > frameTime) {
           SDL_Delay(frameDelay - frameTime);
        }
        lastFrameTime = SDL_GetTicks64();
        deltaTime = (lastFrameTime - frameStartTime) / 1000.0;
    }

    ShutdownGame();
    return 0;
}
