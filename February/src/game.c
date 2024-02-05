#include "game.h"

#include <stdio.h>

static Game *game;
static b8 isActive;

b8 InitializeGame() {
    game = malloc(sizeof(Game));
    game->x = SDL_WINDOWPOS_CENTERED;
    game->y = SDL_WINDOWPOS_CENTERED;
    game->w = 1280;
    game->h = 720;

    game->name = "KayoBreaker";

    SDL_Init(SDL_INIT_EVERYTHING);

    game->window = SDL_CreateWindow(game->name, game->x, game->y, game->w, game->h, SDL_WINDOW_SHOWN);
    if(!game->window) {
        printf_s("Window creation failed: %s\n", SDL_GetError());
        return FALSE;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, 0);
    if(!game->renderer) {
        printf_s("Renderer creation failed: %s\n", SDL_GetError());
        return FALSE;
    }

    TTF_Init();

    game->font = TTF_OpenFont("assets/fonts/ReffsPixelFont.ttf", 32);
    if(!game->font) {
        printf_s("Font loading failed: %s\n", TTF_GetError());
        return FALSE;
    }


    game->isRunning = TRUE;
    isActive = TRUE;

    return TRUE;
}

Game* GetGame() {
    if (isActive) {
        return game;
    }
    return NULL;
}

void UpdateGame() {
    SDL_Event ev;
    SDL_PollEvent(&ev);
    switch(ev.type) {
        case SDL_QUIT:
            game->isRunning = FALSE;
            break;
        case SDL_KEYUP:
            if(ev.key.keysym.sym == SDLK_ESCAPE) {
                game->isRunning = FALSE;
                break;
            }
    }
};


void RenderGame() {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 20, 255);
    SDL_RenderClear(game->renderer);

    SDL_RenderPresent(game->renderer);
}

void ShutdownGame() {
    free(game);
    isActive = FALSE;
}