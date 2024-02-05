#pragma once

#include "defines.h"
#include "SDL.h"
#include "SDL_ttf.h"

typedef struct Paddle {
    f32 x, y;
} Paddle;

typedef struct Ball {
    f32 x, y;
    f32 r;
} Ball;

typedef struct Brick {
    f32 x, y;
    i32 hits;
} Brick;

typedef struct Scene {
    void *internalData;
    b8 isActive;
} Scene;

typedef struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
    b8 isRunning;

    TTF_Font *font;

    char *name;
    u32 x, y, w, h;

    Scene *gameScene;
    Scene *menuScene;

    u32 score;
    u32 highscore;

    b8 gameover;
} Game;