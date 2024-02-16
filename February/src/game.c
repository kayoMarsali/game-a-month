#include "game.h"
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

    game->paddle = malloc(sizeof(Object));
    if(!game->paddle) {
        printf_s("failed to allocate memory for paddle object.");
        return FALSE;
    }
    SDL_memset(game->paddle, 0, sizeof(Object));
    InitializePaddle(game->paddle);
    
    game->ball = malloc(sizeof(Object));
    if(!game->ball) {
        printf_s("failed to allocate memory for ball object.");
        return FALSE;
    }
    SDL_memset(game->ball, 0, sizeof(Object));
    InitializeBall(game->ball); 

    game->brickRows = 7;
    game->brickColumns = 15;
    game->bricks = malloc(sizeof(Object)*(game->brickColumns * game->brickRows));
    if(!game->bricks) {
        printf_s("failed to allocate memory for brick list.");
        return FALSE;
    }
    SDL_memset(game->bricks, 0, sizeof(Object)*(game->brickColumns * game->brickRows));
    for (i16 y = 0; y < game->brickRows; y++)
    {
        for (i16 x = 0; x < game->brickColumns; x++)
        {
            i32 brickIndex = x+y*game->brickColumns;
            Object *brickObj = &game->bricks[brickIndex];
            brickObj->rect.w = 32;
            brickObj->rect.h = 32;
            brickObj->rect.x = 150+(x*brickObj->rect.w);
            brickObj->rect.y = 50+(y*brickObj->rect.h);

            InitializeBrick(brickObj, brickIndex%4);
        }
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
            HandleInput(ev);
            break;
    }
}

void UpdateGame(float deltaTime) {
    PollEvents();
    game->paddle->Update(deltaTime, game->paddle);
    game->ball->Update(deltaTime, game->ball);
    for (i16 y = 0; y < game->brickRows; y++)
    {
        for (i16 x = 0; x < game->brickColumns; x++)
        {
            i32 brickIndex = x+y*game->brickColumns;
            Object *brickObj = &game->bricks[brickIndex];
            brickObj->Update(deltaTime, brickObj);
        }
    }
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

i32 GetPaddleVel() {
    return GetPaddleVelocity(game->paddle);
}


void RenderGame() {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 20, 255);
    SDL_RenderClear(game->renderer);
    
    game->paddle->Render(game->paddle);
    game->ball->Render(game->ball);
    for (i16 y = 0; y < game->brickRows; y++)
    {
        for (i16 x = 0; x < game->brickColumns; x++)
        {
            Object *brickObj = &game->bricks[x+y*game->brickColumns];
            game->bricks->Render(brickObj);
        }
    }
    

    SDL_RenderPresent(game->renderer);
}

void ShutdownGame() {
    for (i16 y = 0; y < game->brickRows; y++)
    {
        for (i16 x = 0; x < game->brickColumns; x++)
        {
            Object *brickObj = &game->bricks[x+y*game->brickColumns];
            DestroyBrick(brickObj);
        }
    }
    free(game->bricks);
    DestroyBall(game->ball);
    free(game->ball);
    DestroyPaddle(game->paddle);
    free(game->paddle);
    TTF_CloseFont(game->font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    free(game);
    isActive = FALSE;
}
