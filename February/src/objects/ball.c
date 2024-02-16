#include "ball.h"
#include "game.h"
#include "brick.h"

typedef struct BallObject {
    i8 ballsLeft;
    b8 ballReleased;
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

        SDL_memset(outBall->objectData, 0, sizeof(BallObject));
    BallObject *ballData = (BallObject *)outBall->objectData;
    outBall->rect.w = 15;
    outBall->rect.h = 15;
    outBall->rect.x = GetGame()->paddle->rect.x+(GetGame()->paddle->rect.w/2)-8;
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

    ballData->ballsLeft = 3;

    return TRUE;
}

b8 IsColliding(Object *brick, Object *ball) {
    if(!brick->isActive) {
        return FALSE;
    }
    if(GetGame()->ballCollided) {
        //ball has already collided with something, quit.
        return FALSE;
    }
    i32 ballCenterX = (ball->rect.x+ball->rect.w/2);
    i32 ballCenterY = (ball->rect.y+ball->rect.h/2);
    i32 ballRadius = ball->rect.w/2+1;
    i32 brickX = brick->rect.x;
    i32 brickY = brick->rect.y;

    BallObject *ballData = (BallObject *)ball->objectData;
    

    //Top Left
    i32 distX = SDL_abs(brickX - ballCenterX);
    i32 distY = SDL_abs(brickY - ballCenterY);
    f32 distance = 0;

    if(distX <= ballRadius) {
        ballData->horizontalBounce = TRUE;
    }
    if(distY <= ballRadius) {
        ballData->verticalBounce = TRUE;
    }
    if(ballData->verticalBounce || ballData->horizontalBounce) {
        distance = SDL_sqrtf((distX*distX) + (distY*distY));
        if (distance <= ballRadius) {
            return TRUE;
        }
    }

    //Bottom Left
    distX = SDL_abs(brickX - ballCenterX);
    distY = SDL_abs(brickY+brick->rect.h - ballCenterY);
    if(distX <= ballRadius) {
        ballData->horizontalBounce = TRUE;
    }
    if(distY <= ballRadius) {
        ballData->verticalBounce = TRUE;
    }
    if(ballData->verticalBounce || ballData->horizontalBounce) {
        distance = SDL_sqrtf((distX*distX) + (distY*distY));
        if (distance <= ballRadius) {
            return TRUE;
        }
    }

    //Bottom Right
    distX = SDL_abs(brickX+brick->rect.w - ballCenterX);
    distY = SDL_abs(brickY+brick->rect.h - ballCenterY);
    if(distX <= ballRadius) {
        ballData->horizontalBounce = TRUE;
    }
    if(distY <= ballRadius) {
        ballData->verticalBounce = TRUE;
    }
    if(ballData->verticalBounce || ballData->horizontalBounce) {
        distance = SDL_sqrtf((distX*distX) + (distY*distY));
        if (distance <= ballRadius) {
            return TRUE;
        }
    }

    //Top Right
    distX = SDL_abs(brickX+brick->rect.w - ballCenterX);
    distY = SDL_abs(brickY - ballCenterY);
    if(distX <= ballRadius) {
        ballData->horizontalBounce = TRUE;
    }
    if(distY <= ballRadius) {
        ballData->verticalBounce = TRUE;
    }
    if(ballData->verticalBounce || ballData->horizontalBounce) {
        distance = SDL_sqrtf((distX*distX) + (distY*distY));
        if (distance <= ballRadius) {
            return TRUE;
        }
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
    GetGame()->ballCollided = FALSE;

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

        
        ballObj->rect.x = GetGame()->paddle->rect.x+(GetGame()->paddle->rect.w/2)-8;
        return;
    }
    ballData->verticalBounce = FALSE;
    ballData->horizontalBounce = FALSE;

    for (i16 y = 0; y < GetGame()->brickRows; y++)
    {
        for (i16 x = 0; x < GetGame()->brickColumns; x++)
        {
            i32 brickIndex = x+y*GetGame()->brickColumns;
            Object *brick = &GetGame()->bricks[brickIndex];
            GetGame()->ballCollided = IsColliding(brick, ball);
            if(GetGame()->ballCollided) {
                HitBrick(brick);
                break;
            }
        }
        if(GetGame()->ballCollided) {
                break;
            }
    }
    if(ballData->horizontalBounce && GetGame()->ballCollided) {
        ballData->horizontalVelocity *= -1;
    }
    if(ballData->verticalBounce && GetGame()->ballCollided) {
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
        ballObj->rect.y = GetGame()->paddle->rect.y-ballObj->rect.h;
        ballData->ballReleased = FALSE;
        ballData->ballsLeft -= 1;
    }

    if(((ballObj->rect.y + ballObj->rect.h) > GetGame()->paddle->rect.y) && 
    (ballObj->rect.y < (GetGame()->paddle->rect.y + GetGame()->paddle->rect.h))) {
        if(((ballObj->rect.x + ballObj->rect.w) > GetGame()->paddle->rect.x) &&
        ballObj->rect.x < (GetGame()->paddle->rect.x + GetGame()->paddle->rect.w)) {
            if(ballData->verticalVelocity > 0) {
                ballData->verticalVelocity *= -1;
                ballData->horizontalVelocity += GetPaddleVel()*0.5f;
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