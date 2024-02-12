#pragma once

#include "defines.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

typedef enum ObjectTypes {
    OBJECT_TYPE_NONE = 0,
    OBJECT_TYPE_PADDLE = 1,
    OBJECT_TYPE_BALL = 2,
    OBJECT_TYPE_BRICK = 3,

    OBJECT_TYPE_MAX_TYPES
} ObjectTypes;

typedef struct Object {
    b8 isActive, isRendered;
    ObjectTypes objectType;
    SDL_Rect rect;
    void *objectData;

    void (*Update)(f32, void*);
    void (*Render)(void*);
} Object;

typedef enum SceneTypes {
    SCENE_TYPE_NONE = 0,
    SCENE_TYPE_GAME = 1,
    SCENE_TYPE_MENU = 2,

    SCENE_TYPE_MAX_TYPES
} SceneTypes;

typedef struct Scene {
    b8 isActive, isRendered;
    SceneTypes sceneType;
    void *internalData;

    void (*Update)(f32, void*);
    void (*Render)(void*);
} Scene;

typedef struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
    b8 isRunning;

    TTF_Font *font;

    char *name;
    u32 x, y, w, h;

    Object *paddle;
    Object *ball;

    u32 score;
    u32 highscore;

    b8 gameover;
} Game;
