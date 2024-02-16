#pragma once
#include "defines.h"
#include "game_types.h"




b8 InitializeGame();

Game* GetGame();

KINLINE b8 GameIsRunning() {
   return (NULL != GetGame()) ? GetGame()->isRunning : FALSE;
}

void UpdateGame(float deltaTime);

b8 GetKeyPressed(char key);
b8 GetKeyHeld(char key);
b8 GetKeyReleased(char key);

i32 GetPaddleVel();

void RenderGame();

void ShutdownGame();