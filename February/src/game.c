#include "game.h"
#include "scene/game_scene.h"
#include "scene/title_scene.h"
#include "objects/paddle.h"
#include "objects/ball.h"
#include "objects/brick.h"


Game *game;

static b8 isActive;
static b8 keyWasPressed[256];
static b8 keyIsPressed[256];

#define KeyPressed(x) keyIsPressed[x] && !keyWasPressed[x] 
#define KeyReleased(x) !keyIsPressed[x] && keyWasPressed[x]

void HandleInput(SDL_Event ev);

void ResetGame() {
    DestroyGameScene(game->activeScene);
    DestroyTitleScene(game->secondaryScene);
    SDL_memset(game->activeScene, 0, sizeof(Scene));
    SDL_memset(game->secondaryScene, 0, sizeof(Scene));
    InitializeGameScene(game->activeScene);
    InitializeTitleScene(game->secondaryScene);
}

b8 InitializeGame() {
    game = malloc(sizeof(Game));
    game->x = SDL_WINDOWPOS_CENTERED;
    game->y = SDL_WINDOWPOS_CENTERED;
    game->w = 800;
    game->h = 600;

    game->name = "KayoBreaker";

    SDL_Init(SDL_INIT_EVERYTHING);

    game->window = SDL_CreateWindow(game->name, game->x, game->y, game->w, game->h, SDL_WINDOW_SHOWN);
    if(!game->window) {
        printf_s("Window creation failed: %s\n", SDL_GetError());
        return FALSE;
    }
    isActive = TRUE;

    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf_s("IMG_Init failed: %s\n", IMG_GetError());
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

    game->activeScene = SDL_malloc(sizeof(Scene));
    if(NULL == game->activeScene) {
        printf_s("Failed to allocate memory for active scene");
        return FALSE;
    }
    SDL_memset(game->activeScene, 0, sizeof(Scene));
    if(!InitializeGameScene(game->activeScene)) {
        printf_s("Failed to initialize game scene.");
        return FALSE;
    }
    

    game->secondaryScene = SDL_malloc(sizeof(Scene));
    if(NULL == game->secondaryScene) {
        printf_s("Failed to allocate memory for secondary scene");
        return FALSE;
    }
    SDL_memset(game->secondaryScene, 0, sizeof(Scene));
    if(!InitializeTitleScene(game->secondaryScene)) {
        printf_s("Failed to initialize title scene.");
        return FALSE;
    }

    game->isRunning = TRUE;

    return TRUE;
}

Game* GetGame() {
    if (isActive) {
        return game;
    }
    return NULL;
}

void PollEvents() {
    SDL_memcpy(keyWasPressed, keyIsPressed, sizeof(keyWasPressed));

    SDL_Event ev;
    SDL_PollEvent(&ev);

    switch(ev.type) {
        case SDL_QUIT:
            game->isRunning = FALSE;
            break;
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            if(ev.key.keysym.sym == SDLK_ESCAPE) {
                game->isRunning = FALSE;
                break;
            }
            if(ev.key.keysym.sym == SDLK_r) {
                ResetGame();
            }
            HandleInput(ev);
            break;
    }
}

void UpdateGame(float deltaTime) {
    PollEvents();
    game->activeScene->Update(deltaTime, game->activeScene);
    game->secondaryScene->Update(deltaTime, game->secondaryScene);
};

void HandleInput(SDL_Event ev) {
    switch (ev.type) {
        case SDL_KEYUP:
        case SDL_KEYDOWN:
        SDL_KeyboardEvent key = ev.key;
        if(key.keysym.sym < 256) {
            keyIsPressed[key.keysym.sym] = (key.state == SDL_PRESSED);
        }
        break;
    }
}

b8 GetKeyPressed(char key) {
    return KeyPressed(key);
}
b8 GetKeyHeld(char key) {
    return keyWasPressed[key];
}
b8 GetKeyReleased(char key) {
    return KeyReleased(key);
}


void RenderGame() {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 20, 255);
    SDL_RenderClear(game->renderer);
    
    game->activeScene->Render(game->activeScene);
    game->secondaryScene->Render(game->secondaryScene);
    

    SDL_RenderPresent(game->renderer);
}

void ShutdownGame() {
    if(NULL != game->activeScene) {
        DestroyGameScene(game->activeScene);
        SDL_free(game->activeScene);
        game->activeScene = NULL;
    }
    if(NULL != game->secondaryScene) {
        DestroyTitleScene(game->secondaryScene);
        SDL_free(game->secondaryScene);
        game->secondaryScene = NULL;
    }
    TTF_CloseFont(game->font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    free(game);
    isActive = FALSE;
}
