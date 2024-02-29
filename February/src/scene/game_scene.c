#include "game_scene.h"
#include "game.h"
#include "objects/ball.h"
#include "objects/brick.h"
#include "objects/paddle.h"

/*
Game Scene Text 
*/
const char *livesTextString = "Lives:";
const char *winTextString = "You Win!";
const char *loseTextString = "You Lose.";

typedef struct GameScene {
    b8 gameover;
    Object *paddle;
    Object *ball;

    
    u16 noBricks;
    i32 brickRows;
    i32 brickColumns;
    Object *bricks;

    SDL_Surface *livesSurf;
    SDL_Surface *winSurf;
    SDL_Surface *loseSurf;
} GameScene;

void UpdateGameScene(f32, void*);
void RenderGameScene(void*);

b8 InitializeGameScene(Scene *outScene) {
    if(SCENE_TYPE_NONE != outScene->sceneType || NULL != outScene->internalData) {
        printf_s("This scene has already been initialized, please destroy the scene before reinitializing it.\n");
        return FALSE;
    }

    outScene->sceneType = SCENE_TYPE_GAME;
    outScene->internalData = SDL_malloc(sizeof(GameScene));
    if(NULL == outScene->internalData) {
        printf_s("Failed to allocate nessacary memory for Game Scene.\n");
        return FALSE;
    }
    
    SDL_memset(outScene->internalData, 0, sizeof(GameScene));
    GameScene* game = (GameScene*)outScene->internalData;

    outScene->Update = UpdateGameScene;
    outScene->Render = RenderGameScene;

    game->paddle = SDL_malloc(sizeof(Object));
    if(!game->paddle) {
        printf_s("failed to allocate memory for paddle object.");
        return FALSE;
    }
    SDL_memset(game->paddle, 0, sizeof(Object));
    InitializePaddle(game->paddle);
    
    game->ball = SDL_malloc(sizeof(Object));
    if(!game->ball) {
        printf_s("failed to allocate memory for ball object.");
        return FALSE;
    }
    SDL_memset(game->ball, 0, sizeof(Object));
    InitializeBall(game->ball); 

    game->brickRows = 7;
    game->brickColumns = 15;
    game->bricks = SDL_malloc(sizeof(Object)*(game->brickColumns * game->brickRows));
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

    SDL_Color white = {255, 255, 255, 255};

    game->winSurf = TTF_RenderText_Blended(GetGame()->font, winTextString, white);
    if(!game->winSurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }
    
    game->livesSurf = TTF_RenderText_Blended(GetGame()->font, livesTextString, white);
    if(!game->livesSurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    game->loseSurf = TTF_RenderText_Blended(GetGame()->font, loseTextString, white);
    if(!game->loseSurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    game->noBricks = game->brickColumns*game->brickRows;
    game->gameover = FALSE;
}
void DestroyGameScene(Scene *scene) {
    if(SCENE_TYPE_GAME != scene->sceneType) {
        printf_s("unexpected scene type.");
        return;
    }
    GameScene* gameScene = (GameScene*)scene->internalData;
    if(NULL != gameScene) {
        SDL_FreeSurface(gameScene->loseSurf);
        SDL_FreeSurface(gameScene->livesSurf);
        SDL_FreeSurface(gameScene->winSurf);
        if(NULL != gameScene->bricks) {
            for (i16 y = 0; y < gameScene->brickRows; y++)
            {
                for (i16 x = 0; x < gameScene->brickColumns; x++)
                {
                    Object *brickObj = &gameScene->bricks[x+y*gameScene->brickColumns];
                    DestroyBrick(brickObj);
                }
            }
            SDL_free(gameScene->bricks);
            gameScene->bricks = NULL;
        }
        if(NULL != gameScene->ball) {
            DestroyBall(gameScene->ball);
            SDL_free(gameScene->ball);
            gameScene->ball = NULL;
        }
        if(NULL != gameScene->paddle) {
            DestroyPaddle(gameScene->paddle);
            SDL_free(gameScene->paddle);
            gameScene->paddle = NULL;
        }

        SDL_free(scene->internalData);
        scene->internalData = NULL;
    }
}

void UpdateGameScene(f32 deltaTime, void* scene){
    Scene *gameScene = (Scene *)scene;
    if(SCENE_TYPE_GAME != gameScene->sceneType) {
        printf_s("Scene Type does not match expected type.");
        return;
    }
    if(!gameScene->isActive) {
        return;
    }
    GameScene* gameData = (GameScene*)gameScene->internalData;
    gameData->gameover = ((GetBallsLeft(gameData->ball) <= 0) || (gameData->noBricks <= 0));
    if(!gameData->gameover) {
        gameData->paddle->Update(deltaTime, gameData->paddle);
        gameData->ball->Update(deltaTime, gameData->ball);
        for (i16 y = 0; y < gameData->brickRows; y++)
        {
            for (i16 x = 0; x < gameData->brickColumns; x++)
            {
                i32 brickIndex = x+y*gameData->brickColumns;
                Object *brickObj = &gameData->bricks[brickIndex];
                brickObj->Update(deltaTime, brickObj);
            }
        }
    }
}

void RenderGameScene(void* scene) {
    Scene *gameScene = (Scene *)scene;
    if(SCENE_TYPE_GAME != gameScene->sceneType) {
        printf_s("Scene Type does not match expected type.");
        return;
    }
    GameScene *gameData = (GameScene*)gameScene->internalData;
    if(!gameScene->isRendered) {
        return;
    }
    SDL_Texture *text = NULL;
    if(!gameData->gameover) {
        text = SDL_CreateTextureFromSurface(GetGame()->renderer, gameData->livesSurf);
        SDL_RenderCopy(GetGame()->renderer, text, NULL, &(SDL_Rect){GetGame()->w - (gameData->ball->rect.w*3+20) - gameData->livesSurf->w,
                                                                    GetGame()->h - gameData->livesSurf->h,
                                                                    gameData->livesSurf->w,
                                                                    gameData->livesSurf->h});
        SDL_DestroyTexture(text);
        text = NULL;
        gameData->paddle->Render(gameData->paddle);
        gameData->ball->Render(gameData->ball);
        for (i16 y = 0; y < gameData->brickRows; y++)
        {
            for (i16 x = 0; x < gameData->brickColumns; x++)
            {
                i32 brickIndex = x+y*gameData->brickColumns;
                Object *brickObj = &gameData->bricks[brickIndex];
                brickObj->Render(brickObj);
            }
        }

        return;
    }
    
    if(GetBallsLeft(gameData->ball) <= 0) {
        text = SDL_CreateTextureFromSurface(GetGame()->renderer, gameData->loseSurf);
        SDL_RenderCopy(GetGame()->renderer, text, NULL, &(SDL_Rect){GetGame()->w/2 - gameData->loseSurf->w/2,
                                                                    GetGame()->h/2 - gameData->loseSurf->h/2,
                                                                    gameData->loseSurf->w,
                                                                    gameData->loseSurf->h});
        SDL_DestroyTexture(text);
        return;
    } else {
        text = SDL_CreateTextureFromSurface(GetGame()->renderer, gameData->winSurf);
        SDL_RenderCopy(GetGame()->renderer, text, NULL, &(SDL_Rect){GetGame()->w/2 - gameData->winSurf->w/2,
                                                                    GetGame()->h/2 - gameData->winSurf->h/2,
                                                                    gameData->winSurf->w,
                                                                    gameData->winSurf->h});
        SDL_DestroyTexture(text);
        return;
    }
}

const Object const* GetPaddle(void *scene) {
    Scene *gameScene = (Scene *)scene;
    if(SCENE_TYPE_GAME != gameScene->sceneType) {
        printf_s("Scene Type does not match expected type.");
        return NULL;
    }
    GameScene *gameData = (GameScene *)gameScene->internalData;

    return gameData->paddle;
}

i32 GetPaddleVel(void *scene) {
    Scene *gameScene = (Scene *)scene;
    if(SCENE_TYPE_GAME != gameScene->sceneType) {
        printf_s("Scene Type does not match expected type.");
        return 0;
    }
    GameScene *gameData = (GameScene *)gameScene->internalData;

    return GetPaddleVelocity(gameData->paddle);
}

void GetBrickInfo(void *scene, BrickInfo *outBrickInfo) {
    Scene *gameScene = (Scene *)scene;
    if(SCENE_TYPE_GAME != gameScene->sceneType) {
        printf_s("Scene Type does not match expected type.");
    }
    GameScene *gameData = (GameScene *)gameScene->internalData;

    outBrickInfo->brickRows = gameData->brickRows;
    outBrickInfo->brickColumns = gameData->brickColumns;
    outBrickInfo->bricks = gameData->bricks;

}

i32 GetLivesSurfH(void *scene) {
    Scene *gameScene = (Scene *)scene;
    if(SCENE_TYPE_GAME != gameScene->sceneType) {
        printf_s("Scene Type does not match expected type.");
    }
    GameScene *gameData = (GameScene *)gameScene->internalData;

    return gameData->livesSurf->h;
}
i32 GetLivesSurfW(void *scene) {
    Scene *gameScene = (Scene *)scene;
    if(SCENE_TYPE_GAME != gameScene->sceneType) {
        printf_s("Scene Type does not match expected type.");
    }
    GameScene *gameData = (GameScene *)gameScene->internalData;

    return gameData->livesSurf->w;
}

void DecBricks(void *scene) {
    Scene *gameScene = (Scene *)scene;
    if(SCENE_TYPE_GAME != gameScene->sceneType) {
        printf_s("Scene Type does not match expected type.");
    }
    GameScene *gameData = (GameScene *)gameScene->internalData;

    gameData->noBricks--;
}