#include "SDL_image.h"
#include "game.h"
#include "paddle.h"



typedef struct PaddleObject {
    u32 posX, posY;
    i32 width, height;
    i32 horizontalVelocity;
    SDL_Surface *sprite;
} PaddleObject;

void RenderPaddle(void *objectData);

b8 InitializePaddle(Object *outPaddle) {
    if(outPaddle->objectType != OBJECT_TYPE_NONE || outPaddle->objectData != NULL) {
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
    paddleData->posX = SDL_WINDOWPOS_CENTERED;
    paddleData->posY = GetGame()->h - 100;
    paddleData->width = 128;
    paddleData->height = 32;

    char *filepath = getcwd(NULL, 260);
    if(NULL == filepath) {
        printf_s("could not get cwd for filepath");
        return FALSE;
    }
    strcat_s(filepath, 260, "\\assets\\sprites\\PaddleTest.png");
    printf_s("%s\n", filepath);
    
    paddleData->sprite = IMG_Load(filepath);
    if(NULL == paddleData->sprite) {
        printf_s("Failed to load image: %s\n", IMG_GetError());
        free(filepath);
        return FALSE;
    }

    free(filepath);
    outPaddle->Render = RenderPaddle;

    return TRUE;
}

void UpdatePaddle(f32 deltaTime, void *objectData) {

}

void RenderPaddle(void *objectData) {
    
}

void DestroyPaddle(Object *paddle) {
    PaddleObject *paddleData = (PaddleObject *)paddle->objectData;
    SDL_FreeSurface(paddleData->sprite);
    SDL_memset(paddleData, 0, sizeof(PaddleObject));

    free(paddle->objectData);
    SDL_memset(paddle, 0, sizeof(Object));
}
