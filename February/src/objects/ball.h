#pragma once

#include "defines.h"
#include "game_types.h"

b8 InitializeBall(Object *outBall);
void DestroyBall(Object *ball);

i32 GetBallsLeft(Object *ball);
