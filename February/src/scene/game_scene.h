#pragma once

#include "defines.h"
#include "objects/brick.h"
#include "game_types.h"

b8 InitializeGameScene(Scene *outScene);
void DestroyGameScene(Scene *scene);

const Object const* GetPaddle(void *scene);
i32 GetPaddleVel(void *internalData);

void GetBrickInfo(void *scene, BrickInfo *outBrickInfo);

i32 GetLivesSurfH(void *scene);
i32 GetLivesSurfW(void *scene);
void DecBricks(void *scene);