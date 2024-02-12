#include "ball.h"
#include "game.h"

typedef struct BallObject {
    b8 ballReleased;
    i32 horizontalVelocity, verticalVelocity;
    SDL_Texture *sprite;
} BallObject;


void UpdateBall(f32 deltaTime, void *ball);
void RenderBall(void *ball);

b8 InitializeBall(Object *outBall) {
    if(OBJECT_TYPE_NONE != outBall->objectType || NULL != outBall->objectData) {
        printf_s("This object has already been initialized, please destroy the object before reinitializing it.\n");
        return FALSE;
    }

    outBall->objectType = OBJECT_TYPE_BALL;
    outBall->objectData = malloc(sizeof(BallObject));
    if(NULL == outBall->objectData) {
        printf_s("Failed to allocate nessacary memory for Ball Object.\n");
        return FALSE;
    }

        SDL_memset(outBall->objectData, 0, sizeof(BallObject));
    BallObject *ballData = (BallObject *)outBall->objectData;
    outBall->rect.w = 15;
    outBall->rect.h = 15;
    outBall->rect.x = GetGame()->paddle->rect.x+64-8;
    outBall->rect.y = GetGame()->paddle->rect.y-15;
    SDL_Surface *sprite = IMG_Load( ".\\assets\\sprites\\BallTest.png");
    if(NULL == sprite) {
        printf_s("Failed to load image: %s\n", IMG_GetError());
        return FALSE;
    }
    ballData->sprite = SDL_CreateTextureFromSurface(GetGame()->renderer, sprite);
    if(NULL == ballData->sprite) {
        printf_s("Failed to create sprite texture: %s\n", SDL_GetError());
        return FALSE;
    }
    SDL_FreeSurface(sprite);

    outBall->Update = UpdateBall;
    outBall->Render = RenderBall;

    return TRUE;
}

void UpdateBall(f32 deltaTime, void *ball) {
    Object *ballObj = (Object *)ball;
    if(OBJECT_TYPE_BALL != ballObj->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }
    BallObject *ballData = (BallObject *)ballObj->objectData;
    if(!ballData->ballReleased) {
         if(GetKeyHeld('d')) {
            ballData->horizontalVelocity = 200;
        } else if (GetKeyHeld('a')) {
            ballData->horizontalVelocity = -200;
        } else {
            ballData->horizontalVelocity = 0;
        }
        
        if(GetKeyPressed(' ')) {
            ballData->ballReleased = TRUE;
            ballData->verticalVelocity = -100;
        }

        
        ballObj->rect.x = GetGame()->paddle->rect.x+64-8;
    } else {
        ballObj->rect.x += ballData->horizontalVelocity * deltaTime;
        ballObj->rect.y += ballData->verticalVelocity * deltaTime;

        if((ballObj->rect.x < 0) || (ballObj->rect.x + ballObj->rect.w >= GetGame()->w)) {
            ballData->horizontalVelocity *= -1;
        }
        if(ballObj->rect.y < 0) {
            ballData->verticalVelocity *= -1;
        } else if(ballObj->rect.y > GetGame()->h) {
            ballObj->rect.y = GetGame()->paddle->rect.y-ballObj->rect.h;
            ballData->ballReleased = FALSE;
            //TODO: remove a life.
        }

        //TODO: paddle collision.
    }
}
void RenderBall(void *ball) {
    Object *ballObj = (Object *)ball;
    if(OBJECT_TYPE_BALL != ballObj->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }

    BallObject *ballData = (BallObject *)ballObj->objectData;

    SDL_RenderCopy(GetGame()->renderer, ballData->sprite, NULL, &ballObj->rect);
}


void DestroyBall(Object *ball) {
    if(OBJECT_TYPE_BALL != ball->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }
    BallObject *ballData = (BallObject *)ball->objectData;
    SDL_memset(ballData, 0, sizeof(BallObject));

    free(ball->objectData);
    SDL_memset(ball, 0, sizeof(Object));

}