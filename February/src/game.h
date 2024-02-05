#pragma once

#include "defines.h"
#include "game_types.h"

b8 InitializeGame();

Game* GetGame();

KINLINE b8 GameIsRunning() {
   return (GetGame() != NULL) ? GetGame()->isRunning : FALSE;
}

void UpdateGame();
void RenderGame();

void ShutdownGame();