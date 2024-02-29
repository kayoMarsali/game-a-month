#include "SDL_image.h"
#include "game.h"
#include "paddle.h"



typedef struct PaddleObject {
    i32 horizontalVelocity;
    SDL_Texture *sprite;
} PaddleObject;

void UpdatePaddle(f32 deltaTime, void *paddle);
void RenderPaddle(void *paddle);

b8 InitializePaddle(Object *outPaddle) {
    if(OBJECT_TYPE_NONE != outPaddle->objectType || NULL != outPaddle->objectData) {
        printf_s("This object has already been initialized, please destroy the object before reinitializing it.\n");
        return FALSE;
    }

    outPaddle->objectType = OBJECT_TYPE_PADDLE;
    outPaddle->objectData = malloc(sizeof(PaddleObject));
    if(NULL == outPaddle->objectData) {
        printf_s("Failed to allocate nessacary memory for Paddle Object.\n");
        return FALSE;
    }

    SDL_memset(outPaddle->objectData, 0, sizeof(PaddleObject));
    PaddleObject *paddleData = (PaddleObject *)outPaddle->objectData;
    outPaddle->rect.w = 64;
    outPaddle->rect.h = 16;
    outPaddle->rect.x = (GetGame()->w / 2) - (outPaddle->rect.w / 2);
    outPaddle->rect.y = GetGame()->h - 100;
    SDL_Surface *sprite = IMG_Load( ".\\assets\\sprites\\PaddleTest.png");
    if(NULL == sprite) {
        printf_s("Failed to load image: %s\n", IMG_GetError());
        return FALSE;
    }
    paddleData->sprite = SDL_CreateTextureFromSurface(GetGame()->renderer, sprite);
    if(NULL == paddleData->sprite) {
        printf_s("Failed to create sprite texture: %s\n", SDL_GetError());
        return FALSE;
    }
    SDL_FreeSurface(sprite);

    outPaddle->Update = UpdatePaddle;
    outPaddle->Render = RenderPaddle;

    return TRUE;
}

void UpdatePaddle(f32 deltaTime, void *paddle) {
    Object *paddleObj = (Object *)paddle;
    if(OBJECT_TYPE_PADDLE != paddleObj->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }
    PaddleObject *paddleData = (PaddleObject *)paddleObj->objectData;
    
    if(GetKeyHeld('d')) {
        paddleData->horizontalVelocity = 200;
    } else if (GetKeyHeld('a')) {
        paddleData->horizontalVelocity = -200;
    } else {
        paddleData->horizontalVelocity = 0;
    }

    if(paddleData->horizontalVelocity) {
        paddleObj->rect.x += paddleData->horizontalVelocity * deltaTime;
    }

    if(paddleObj->rect.x < 0) {
        paddleObj->rect.x = 0;
    }
    if(paddleObj->rect.x + paddleObj->rect.w > GetGame()->w + 1) {
        paddleObj->rect.x = GetGame()->w - paddleObj->rect.w;
    }
}

void RenderPaddle(void *paddle) {
    Object *paddleObj = (Object *)paddle;
    if(OBJECT_TYPE_PADDLE != paddleObj->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }
    PaddleObject *paddleData = (PaddleObject *)paddleObj->objectData;

    SDL_RenderCopy(GetGame()->renderer, paddleData->sprite, NULL, &paddleObj->rect);
}

void DestroyPaddle(Object *paddle) {
    if(OBJECT_TYPE_PADDLE != paddle->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }
    PaddleObject *paddleData = (PaddleObject *)paddle->objectData;
    SDL_memset(paddleData, 0, sizeof(PaddleObject));

    free(paddle->objectData);
    SDL_memset(paddle, 0, sizeof(Object));
}

i32 GetPaddleVelocity(Object *paddle) {
    if(OBJECT_TYPE_PADDLE != paddle->objectType) {
        printf_s("Object Type does not match expected type.");
        return 0;
    }
    PaddleObject *paddleData = (PaddleObject *)paddle->objectData;

    return paddleData->horizontalVelocity;
}