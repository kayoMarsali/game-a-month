#include "brick.h"
#include "ball.h"
#include "game.h"
#include "scene/game_scene.h"

typedef struct BrickObject {
    i8 hits;
    b8 isHit;
    SDL_Texture *sprite;
} BrickObject;


void UpdateBrick(f32 deltaTime, void *brick);
void RenderBrick(void *brick);

b8 InitializeBrick(Object *outBrick, u8 spriteIndex) {
    if(OBJECT_TYPE_NONE != outBrick->objectType || NULL != outBrick->objectData) {
        printf_s("This object has already been initialized, please destroy the object before reinitializing it.\n");
        return FALSE;
    }

    outBrick->objectType = OBJECT_TYPE_BRICK;
    outBrick->objectData = malloc(sizeof(BrickObject));
    if(NULL == outBrick->objectData) {
        printf_s("Failed to allocate nessacary memory for Brick Object.\n");
        return FALSE;
    }

        SDL_memset(outBrick->objectData, 0, sizeof(BrickObject));
    BrickObject *brickData = (BrickObject *)outBrick->objectData;
    SDL_Surface *sprite = NULL;
    switch(spriteIndex) {
        case 0:
            sprite = IMG_Load( ".\\assets\\sprites\\BrickTest01.png");
            break;
        case 1:
            sprite = IMG_Load( ".\\assets\\sprites\\BrickTest02.png");
            break;
        case 2:
            sprite = IMG_Load( ".\\assets\\sprites\\BrickTest03.png");
            break;
        case 3:
            sprite = IMG_Load( ".\\assets\\sprites\\BrickTest04.png");
            break;
        default:
            printf_s("spriteIndex out of bounds: %d", spriteIndex);
            return FALSE;
    }
    if(NULL == sprite) {
        printf_s("Failed to load image: %s\n", IMG_GetError());
        return FALSE;
    }
    brickData->sprite = SDL_CreateTextureFromSurface(GetGame()->renderer, sprite);
    if(NULL == brickData->sprite) {
        printf_s("Failed to create sprite texture: %s\n", SDL_GetError());
        return FALSE;
    }
    SDL_FreeSurface(sprite);

    brickData->hits = 1;

    outBrick->Update = UpdateBrick;
    outBrick->Render = RenderBrick;

    outBrick->isActive = TRUE;
    outBrick->isRendered = TRUE;

    return TRUE;
}

void UpdateBrick(f32 deltaTime, void *brick) {
    Object *brickObj = (Object *)brick;
    if(OBJECT_TYPE_BRICK != brickObj->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }
    BrickObject *brickData = (BrickObject *)brickObj->objectData;
    if(!brickObj->isActive) {
        return;
    }
    if(brickData->isHit) {
        
        brickData->hits-=1;
    }

    if(brickData->hits == 0) {
        brickObj->isActive = FALSE;
        brickObj->isRendered = FALSE;
        DecBricks(GetGame()->activeScene);
    }

    brickData->isHit = FALSE;
}
void RenderBrick(void *brick) {
    Object *brickObj = (Object *)brick;
    if(OBJECT_TYPE_BRICK != brickObj->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }

    BrickObject *brickData = (BrickObject *)brickObj->objectData;
    if(!brickObj->isRendered) {
        return;
    }
    SDL_RenderCopy(GetGame()->renderer, brickData->sprite, NULL, &brickObj->rect);
}

void HitBrick(Object *brick) {
    if(OBJECT_TYPE_BRICK != brick->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }
    BrickObject *brickData = (BrickObject *)brick->objectData;

    brickData->isHit = TRUE;
}

void DestroyBrick(Object *brick) {
    if(OBJECT_TYPE_BRICK != brick->objectType) {
        printf_s("Object Type does not match expected type.");
        return;
    }
    BrickObject *brickData = (BrickObject *)brick->objectData;
    SDL_memset(brickData, 0, sizeof(BrickObject));

    free(brick->objectData);
    SDL_memset(brick, 0, sizeof(Object));
}