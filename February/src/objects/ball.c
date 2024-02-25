#include "ball.h"
#include "game.h"
#include "scene/game_scene.h"
#include "brick.h"

typedef struct BallObject {
    i8 ballsLeft;
    b8 ballReleased;
    b8 ballCollided;
    b8 horizontalBounce, verticalBounce;
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

    const Object *paddle = GetPaddle(GetGame()->activeScene);
    SDL_memset(outBall->objectData, 0, sizeof(BallObject));

    BallObject *ballData = (BallObject *)outBall->objectData;
    outBall->rect.w = 15;
    outBall->rect.h = 15;
    outBall->rect.x = paddle->rect.x+(paddle->rect.w/2)-8;
    outBall->rect.y = paddle->rect.y-15;
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

    ballData->ballsLeft = 3;

    return TRUE;
}

b8 IsColliding(Object *brick, Object *ball) {
    BallObject *ballData = (BallObject *)ball->objectData;

    if(!brick->isActive) {
        return FALSE;
    }
    if(ballData->ballCollided) {
        //ball has already collided with something, quit.
        return FALSE;
    }
    i32 ballCenterX = (ball->rect.x+ball->rect.w/2);
    i32 ballCenterY = (ball->rect.y+ball->rect.h/2);
    i32 ballRadius = ball->rect.w/2+1;
    i32 brickX = brick->rect.x;
    i32 brickY = brick->rect.y;
    

    i32 distX = SDL_abs(brickX+ballRadius - ballCenterX);
    i32 distY = SDL_abs(brickY - ballCenterY);
    f32 distance = 0;
    distance = SDL_sqrtf((distX*distX) + (distY*distY));
    if (distance <= ballRadius) {
        ballData->verticalBounce = TRUE;
        return TRUE;
    }

    distX = SDL_abs(brickX - ballCenterX);
    distY = SDL_abs(brickY+ballRadius - ballCenterY);
    distance = SDL_sqrtf((distX*distX) + (distY*distY));
    if (distance <= ballRadius) {
        ballData->horizontalBounce = TRUE;
        return TRUE;
    }

    distX = SDL_abs(brickX+brick->rect.w-ballRadius - ballCenterX);
    distY = SDL_abs(brickY - ballCenterY);
    distance = SDL_sqrtf((distX*distX) + (distY*distY));
    if (distance <= ballRadius) {
        ballData->verticalBounce = TRUE;
        return TRUE;
    }


    distX = SDL_abs(brickX+brick->rect.w - ballCenterX);
    distY = SDL_abs(brickY+ballRadius - ballCenterY);
    distance = SDL_sqrtf((distX*distX) + (distY*distY));
    if (distance <= ballRadius) {
        ballData->horizontalBounce = TRUE;
        return TRUE;
    }

    distX = SDL_abs(brickX+ballRadius - ballCenterX);
    distY = SDL_abs(brickY+brick->rect.h - ballCenterY);
    distance = SDL_sqrtf((distX*distX) + (distY*distY));
    if (distance <= ballRadius) {
        ballData->verticalBounce = TRUE;
        return TRUE;
    }
    
    distX = SDL_abs(brickX - ballCenterX);
    distY = SDL_abs(brickY+brick->rect.h-ballRadius - ballCenterY);    
    distance = SDL_sqrtf((distX*distX) + (distY*distY));
    if (distance <= ballRadius) {
        ballData->horizontalBounce = TRUE;
        return TRUE;
    }

    distX = SDL_abs(brickX+brick->rect.w-ballRadius - ballCenterX);
    distY = SDL_abs(brickY+brick->rect.h - ballCenterY);
    distance = SDL_sqrtf((distX*distX) + (distY*distY));
    if (distance <= ballRadius) {
        ballData->verticalBounce = TRUE;
        return TRUE;
    }

    distX = SDL_abs(brickX+brick->rect.w - ballCenterX);
    distY = SDL_abs(brickY+brick->rect.h-ballRadius - ballCenterY);
    distance = SDL_sqrtf((distX*distX) + (distY*distY));
    if (distance <= ballRadius) {
        ballData->horizontalBounce = TRUE;
        return TRUE;
    }


    ballData->horizontalBounce = FALSE;
    ballData->verticalBounce = FALSE;
    return FALSE;
}

void UpdateBall(f32 deltaTime, void *ball) {
    Object *ballObj = (Object *)ball;
    if(OBJECT_TYPE_BALL != ballObj->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }
    BallObject *ballData = (BallObject *)ballObj->objectData;
    ballData->ballCollided = FALSE;
    const Object *paddle = GetPaddle(GetGame()->activeScene);

    if(!ballData->ballReleased) {
         if(GetKeyHeld('d')) {
            ballData->horizontalVelocity = 150;
        } else if (GetKeyHeld('a')) {
            ballData->horizontalVelocity = -150;
        } else {
            ballData->horizontalVelocity = 0;
        }
        
        if(GetKeyPressed(' ')) {
            ballData->ballReleased = TRUE;
            ballData->verticalVelocity = -150;
        }

        
        ballObj->rect.x = paddle->rect.x+(paddle->rect.w/2)-8;
        return;
    }

    ballData->verticalBounce = FALSE;
    ballData->horizontalBounce = FALSE;

    BrickInfo *brickInfo = SDL_malloc(sizeof(BrickInfo));
    GetBrickInfo(GetGame()->activeScene, brickInfo);
    
    for (i16 y = 0; y < brickInfo->brickRows; y++)
    {
        for (i16 x = 0; x < brickInfo->brickColumns; x++)
        {
            i32 brickIndex = x+y*brickInfo->brickColumns;
            Object *brick = &brickInfo->bricks[brickIndex];
            ballData->ballCollided = IsColliding(brick, ball);
            if(ballData->ballCollided) {
                HitBrick(brick);
                break;
            }
        }
        if(ballData->ballCollided) {
                break;
            }
    }

    SDL_free(brickInfo);
    if(ballData->horizontalBounce && ballData->ballCollided) {
        ballData->horizontalVelocity *= -1;
    }
    if(ballData->verticalBounce && ballData->ballCollided) {
        ballData->verticalVelocity *= -1;
    }
    ballObj->rect.x += ballData->horizontalVelocity * deltaTime;
    ballObj->rect.y += ballData->verticalVelocity * deltaTime;

    if((ballObj->rect.x < 0) || (ballObj->rect.x + ballObj->rect.w >= GetGame()->w)) {
        ballData->horizontalVelocity *= -1;
    }
    if(ballObj->rect.y < 0) {
        ballData->verticalVelocity *= -1;
    } else if(ballObj->rect.y > GetGame()->h) {
        ballObj->rect.y = paddle->rect.y-ballObj->rect.h;
        ballData->ballReleased = FALSE;
        ballData->ballsLeft -= 1;
    }

    if(((ballObj->rect.y + ballObj->rect.h) > paddle->rect.y) && 
    (ballObj->rect.y < paddle->rect.y + paddle->rect.h)) {
        if(((ballObj->rect.x + ballObj->rect.w) > paddle->rect.x) &&
        ballObj->rect.x < (paddle->rect.x + paddle->rect.w)) {
            if(ballData->verticalVelocity > 0) {
                ballData->verticalVelocity *= -1;
                ballData->horizontalVelocity += GetPaddleVel(GetGame()->activeScene)*0.5f;
                if(ballData->horizontalVelocity >= 350) {
                    ballData->horizontalVelocity = 350;
                } else if (ballData->horizontalVelocity <= -350) {
                    ballData->horizontalVelocity = -350;
                }
            }
        }
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

    i32 livesStart = GetGame()->w - (ballObj->rect.w+5)*3;
    for(i32 i = 0; i < ballData->ballsLeft; ++i) {
        SDL_RenderCopy(GetGame()->renderer, ballData->sprite, NULL, &(SDL_Rect){livesStart + (ballObj->rect.w+5)*i,
                                                                    GetGame()->h - GetLivesSurfH(GetGame()->activeScene) + 4,
                                                                    ballObj->rect.w,
                                                                    ballObj->rect.h});
    }
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


i32 GetBallsLeft(Object *ball) {
    if(OBJECT_TYPE_BALL != ball->objectType) {
        printf_s("Object Type does not match expected type.");
        return -1;
    }

    return ((BallObject *)ball->objectData)->ballsLeft;
}