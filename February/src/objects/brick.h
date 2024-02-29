#pragma once

#include "defines.h"
#include "game_types.h"

typedef struct BrickInfo {
    i32 brickRows;
    i32 brickColumns;
    Object *bricks;
} BrickInfo;

b8 InitializeBrick(Object *outBrick, u8 spriteIndex);
void HitBrick(Object *brick);
void DestroyBrick(Object *brick);
