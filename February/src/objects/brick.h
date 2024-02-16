#pragma once

#include "defines.h"
#include "game_types.h"

b8 InitializeBrick(Object *outBrick, u8 spriteIndex);
void HitBrick(Object *brick);
void DestroyBrick(Object *brick);
