#include <Windows.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include "defines.h"
#include "SDL.h"
#include "SDL_ttf.h"


#define SPEED_CAP 500
#define FLOAT_TOLERANCE 0.0001f

SDL_Color white = {255, 255, 255, 255};
SDL_Color grey = {128, 128, 128, 255};

SDL_Surface *ResetSurf = NULL;
string ResetString = "Press 'R' to Start and Restart the game";
SDL_Surface *ToggleAISurf = NULL;
string ToggleAIString = "Press 'I' to toggle Player 2 AI";
SDL_Surface *WinSurf = NULL;
string WinString = "You  Win!";
SDL_Surface *LoseSurf = NULL;
string LoseString = "Game Over!";
SDL_Surface *p1Surf = NULL;
string p1String = "P1 - W/S";
SDL_Surface *p2Surf = NULL;
string p2String = "Up/Down - P2";


typedef struct Paddle {
    f32 x, y;
    i32 w, h;
    i32 speed_v;
} Paddle;

typedef struct Ball {
    f32 x, y;
    i32 w, h;
    i32 speed_h, speed_v;
} Ball;

typedef struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
    b8 isRunning;

    TTF_Font *font;

    string name;
    u32 x, y, w, h;

    Ball ball;
    Paddle player1;
    u8 p1Score;
    Paddle player2;
    u8 p2Score;
    b8 p2IsAI;

    b8 gameover;

} Game;

Game *game = NULL;

b8 InitializeGameWindow(void);
void Setup(void);
void PollEvents(void);

void Update(f64 deltaTime);
void UpdateBall(f64 deltaTime);
void UpdatePaddles(f64 deltaTime);
void UpdateScore(void);

void Render(void);
void RenderBoard(void);
void RenderBall(void);
void RenderPaddles(void);
void RenderScore(void);
void RenderText(void);

void DestroyGameWindow(void);

int main(int argc, char** argv) {
    
    if(!InitializeGameWindow()) {
        return -1;
    }

    u64 lastFrameTime = 0;
    u64 frameStartTime = 0;
    u64 frameEndTime = 0;
    u64 frameTime = 0;
    u64 targetFPS = 60;
    u64 frameDelay = 1000 / targetFPS;
    f64 deltaTime = 0.0;

    Setup();
    game->gameover = TRUE;
    while(game->isRunning) {
        frameStartTime = SDL_GetTicks64();
        PollEvents();
        Update(deltaTime);
        Render();

        frameEndTime = SDL_GetTicks64();
        frameTime = frameEndTime - frameStartTime;
        if(frameDelay > frameTime) {
           SDL_Delay(frameDelay - frameTime);
        }
        lastFrameTime = SDL_GetTicks64();
        deltaTime = (lastFrameTime - frameStartTime) / 1000.0;
    }

    DestroyGameWindow();

    return 0;
}

b8 InitializeGameWindow(void) {
     
    game = malloc(sizeof(Game));
    memset(game, 0, sizeof(Game));

    game->x = SDL_WINDOWPOS_CENTERED;
    game->y = SDL_WINDOWPOS_CENTERED;
    game->w = 1280;
    game->h = 720;

    game->name = "KayoPong";

    SDL_Init(SDL_INIT_EVERYTHING);

    game->window = SDL_CreateWindow(game->name, game->x, game->y, game->w, game->h, SDL_WINDOW_SHOWN);
    if(!game->window) {
        printf_s("Window creation failed: %s\n", SDL_GetError());
        return FALSE;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, 0);
    if(!game->renderer) {
        printf_s("Renderer creation failed: %s\n", SDL_GetError());
        return FALSE;
    }

    TTF_Init();

    game->font = TTF_OpenFont("assets/fonts/OpenSans-Regular.ttf", 35);
    if(!game->font) {
        printf_s("Font loading failed: %s\n", TTF_GetError());
        return FALSE;
    }

    ResetSurf = TTF_RenderText_Blended(game->font, ResetString, white);
    if(!ResetSurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    ToggleAISurf = TTF_RenderText_Blended(game->font, ToggleAIString, white);
    if(!ToggleAISurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    WinSurf = TTF_RenderText_Blended(game->font, WinString, white);
    if(!WinSurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    LoseSurf = TTF_RenderText_Blended(game->font, LoseString, white);
    if(!LoseSurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    p1Surf = TTF_RenderText_Blended(game->font, p1String, white);
    if(!p1Surf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    p2Surf = TTF_RenderText_Blended(game->font, p2String, white);
    if(!p2Surf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }


    game->isRunning = TRUE;

    return TRUE;
}

void Setup(void) {
    srand(SDL_GetTicks());

    game->ball.x = game->w / 2;
    game->ball.y = game->h / 2;

    game->ball.w = 15; 
    game->ball.h = 15;

    game->ball.speed_h = 200;
    game->ball.speed_v = ((rand() % 10) - 5) * 20;

    game->player1.x = 100;
    game->player1.y = game->h/2;

    game->player1.w = 15;
    game->player1.h = 63;

    game->player1.speed_v = 0;
    game->p1Score = 0;

    game->player2.x = game->w - 115;
    game->player2.y = game->h/2;

    game->player2.w = 15;
    game->player2.h = 63;

    game->player2.speed_v = 0;
    game->p2Score = 0;

    game->gameover = FALSE;
}

void PollEvents(void) {
    SDL_Event ev;
    SDL_PollEvent(&ev);
    switch(ev.type) {
        case SDL_QUIT:
            game->isRunning = FALSE;
            break;
        case SDL_KEYUP:
            if(ev.key.keysym.sym == SDLK_ESCAPE) {
                game->isRunning = FALSE;
            }
            if(ev.key.keysym.sym == SDLK_w || ev.key.keysym.sym == SDLK_s) {
                game->player1.speed_v = 0;
            }
            if(ev.key.keysym.sym == SDLK_r) {
                Setup();
            }
            if(ev.key.keysym.sym == SDLK_i) {
                game->p2IsAI = !game->p2IsAI;
            }
            if(ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_DOWN) {
                if(!game->p2IsAI) {
                    game->player2.speed_v = 0;
                }
            }
            break;
        case SDL_KEYDOWN:
            if(ev.key.keysym.sym == SDLK_w) {
                game->player1.speed_v = -175;
            }
            if(ev.key.keysym.sym == SDLK_s) {
                game->player1.speed_v = 175;
            }
            if(ev.key.keysym.sym == SDLK_UP) {
                if(!game->p2IsAI) {
                    game->player2.speed_v = -175;
                }
            }
            if(ev.key.keysym.sym == SDLK_DOWN) {
                if(!game->p2IsAI) {
                    game->player2.speed_v = 175;
                }
            }
            break;
        default:
            break;
    }
}
void Update(f64 deltaTime) {
    if(game->gameover) {
        return;
    }

    UpdatePaddles(deltaTime);
    UpdateBall(deltaTime);
    UpdateScore();

}
void UpdateBall(f64 deltaTime) {
    if((game->ball.x+game->ball.w > 100) && ((game->ball.x) < game->w-100)) {
        if((((game->ball.y + game->ball.h) > (game->h-50)) && game->ball.speed_v > 0) || ((game->ball.y < 50) && game->ball.speed_v < 0)) {
            game->ball.speed_v*=-1.00f;
        }
    }

        if((game->ball.x < (game->player1.x + game->player1.w)) && (game->ball.x > game->player1.x)) {
        if(((game->ball.y+game->ball.h) < (game->player1.y + game->player1.h)) && (game->ball.y+15 > game->player1.y)) {
            if(game->ball.speed_h < 0) {
                game->ball.speed_h *= -1.25f;
                game->ball.speed_v += game->player1.speed_v/2;
                return;
            }
        }
    }

    if(((game->ball.x+game->ball.w) < (game->player2.x + game->player2.w)) && ((game->ball.x + game->ball.w) > game->player2.x)) {
        if(((game->ball.y+game->ball.h) < (game->player2.y + game->player2.h)) && (game->ball.y+15 > game->player2.y)) {
            if(game->ball.speed_h > 0) {
                game->ball.speed_h *= -1.25f;
                game->ball.speed_v += game->player2.speed_v/2;
                return;
            }
        }
    }
    
    game->ball.x += game->ball.speed_h * deltaTime;
    game->ball.y += game->ball.speed_v * deltaTime;
}

void UpdatePaddles(f64 deltaTime) {

    game->player1.y += game->player1.speed_v * deltaTime;
    if(game->player1.y < 50) {
        game->player1.y = 50;
    }
    if(game->player1.y > game->h-game->player1.h-50) {
        game->player1.y = (game->h-game->player1.h-50);
    }

    if(game->p2IsAI) {
        f32 dist = (game->player2.y+game->player2.h/2) - (game->ball.y+game->ball.h/2);
        
        if(dist < 15 && dist > -15) {
            game->player2.speed_v = 0;
        } else {
            if(dist < 0) {
                game->player2.speed_v = 150;
            }
            if(dist < 50) {
                game->player2.speed_v = 300;
            }
            if(dist > 0) {
                game->player2.speed_v = -150;
            }
            if(dist > 50) {
                game->player2.speed_v = -300;
            }
        }
    }

    game->player2.y += game->player2.speed_v * deltaTime;
    if(game->player2.y < 50) {
        game->player2.y = 50;
    }
    if(game->player2.y > game->h-game->player2.h-50) {
        game->player2.y = (game->h-game->player2.h-50);
    }
}

void UpdateScore(void) {
    if(game->ball.x > game->w) {
        game->p1Score++;
        game->ball.x = game->w/2;
        game->ball.y = game->h/2;
        game->ball.speed_h = -200;
        game->ball.speed_v = -((rand() % 10) - 5) * 20;
    }

    if(game->ball.x + game->ball.w < 0) {
        game->p2Score++;
        game->ball.x = game->w/2;
        game->ball.y = game->h/2;
        game->ball.speed_h = 200;
        game->ball.speed_v = ((rand() % 10) - 5) * 20;
    }
}

void Render(void) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 20, 255);
    SDL_RenderClear(game->renderer);

    SDL_SetRenderDrawColor(game->renderer, grey.r, grey.g, grey.b, grey.a);
    RenderBoard();
    RenderScore();
    SDL_SetRenderDrawColor(game->renderer, white.r, white.g, white.b, white.a);
    if(!game->gameover) { 
        RenderBall();
        RenderPaddles();
    };
    RenderText();

    SDL_RenderPresent(game->renderer);
}

void RenderBoard(void) {
    SDL_Rect boardRects[2] = {{100, 35, game->w-200, 15}, {100, game->h-50, game->w-200, 15}};
    SDL_Rect middleLineRects[7] = {{game->w/2,  50, 15, 32},
                                    {game->w/2, 135, 15, 64},
                                    {game->w/2, 235, 15, 64},
                                    {game->w/2, 332, 15, 64},
                                    {game->w/2, 429, 15, 64},
                                    {game->w/2, 526, 15, 64},
                                    {game->w/2, 638, 15, 32}};

    SDL_RenderFillRects(game->renderer, boardRects, 2);
    SDL_RenderFillRects(game->renderer, middleLineRects, 7);
}

void RenderBall(void) {
    SDL_Rect ballRect = { game->ball.x, game->ball.y, game->ball.w, game->ball.h };

    SDL_RenderFillRect(game->renderer, &ballRect);
}

void RenderPaddles(void) {
    SDL_Rect p1Rect = {game->player1.x, game->player1.y, game->player1.w, game->player1.h};
    SDL_Rect p2Rect = {game->player2.x, game->player2.y, game->player2.w, game->player2.h};

    SDL_RenderFillRect(game->renderer, &p1Rect);
    SDL_RenderFillRect(game->renderer, &p2Rect);

}

void RenderScore(void) {
    u32 p1ScoreHPos = game->w/4 - 32;
    u32 p2ScoreHPos = game->w - (game->w/4 + 32);
    u32 count = 0;
    SDL_Rect *score = malloc(sizeof(SDL_Rect)*7);;

    SDL_Rect p1ScoreDisplay[7] = {{p1ScoreHPos, 100, 64, 16},
                              {p1ScoreHPos, 100, 16, 48},
                              {p1ScoreHPos + 48, 100, 16, 48},
                              {p1ScoreHPos, 140, 64, 16},
                              {p1ScoreHPos, 140, 16, 48},
                              {p1ScoreHPos + 48, 140, 16, 48},
                              {p1ScoreHPos, 180, 64, 16}};

    switch(game->p1Score) {
        case 0:
            count = 6;
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[1];
            score[2] = p1ScoreDisplay[2];
            score[3] = p1ScoreDisplay[4];
            score[4] = p1ScoreDisplay[5];
            score[5] = p1ScoreDisplay[6];
            break;
        case 1:
            count = 2;
            score[0] = p1ScoreDisplay[2];
            score[1] = p1ScoreDisplay[5];
            break;
        case 2:
            count = 5;
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[2];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[4];
            score[4] = p1ScoreDisplay[6];
            break;
        case 3:
            count = 5;
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[2];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[5];
            score[4] = p1ScoreDisplay[6];
            break;
        case 4:
            count = 4;
            score[0] = p1ScoreDisplay[1];
            score[1] = p1ScoreDisplay[2];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[5];
            break;
        case 5:
            count = 5;
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[1];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[5];
            score[4] = p1ScoreDisplay[6];
            break;
        case 6:
            count = 6;
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[1];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[4];
            score[4] = p1ScoreDisplay[5];
            score[5] = p1ScoreDisplay[6];
            break;
        case 7:
            count = 3;
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[2];
            score[2] = p1ScoreDisplay[5];
            break;
        case 8:
            count = 7;
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[1];
            score[2] = p1ScoreDisplay[2];
            score[3] = p1ScoreDisplay[3];
            score[4] = p1ScoreDisplay[4];
            score[5] = p1ScoreDisplay[5];
            score[6] = p1ScoreDisplay[6];
            break;
        case 9:
            count = 6;
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[1];
            score[2] = p1ScoreDisplay[2];
            score[3] = p1ScoreDisplay[3];
            score[4] = p1ScoreDisplay[5];
            score[5] = p1ScoreDisplay[6];
            break;
        default:
            goto winlabel;
    }

    SDL_RenderFillRects(game->renderer, score, count);
    memset(score, 0, sizeof(SDL_Rect)*7);
    count = 0;

    SDL_Rect p2ScoreDisplay[7] = {{p2ScoreHPos, 100, 64, 16},
                              {p2ScoreHPos, 100, 16, 48},
                              {p2ScoreHPos + 48, 100, 16, 48},
                              {p2ScoreHPos, 140, 64, 16},
                              {p2ScoreHPos, 140, 16, 48},
                              {p2ScoreHPos + 48, 140, 16, 48},
                              {p2ScoreHPos, 180, 64, 16}};

    switch(game->p2Score) {
        case 0:
            count = 6;
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[1];
            score[2] = p2ScoreDisplay[2];
            score[3] = p2ScoreDisplay[4];
            score[4] = p2ScoreDisplay[5];
            score[5] = p2ScoreDisplay[6];
            break;
        case 1:
            count = 2;
            score[0] = p2ScoreDisplay[2];
            score[1] = p2ScoreDisplay[5];
            break;
        case 2:
            count = 5;
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[2];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[4];
            score[4] = p2ScoreDisplay[6];
            break;
        case 3:
            count = 5;
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[2];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[5];
            score[4] = p2ScoreDisplay[6];
            break;
        case 4:
            count = 4;
            score[0] = p2ScoreDisplay[1];
            score[1] = p2ScoreDisplay[2];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[5];
            break;
        case 5:
            count = 5;
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[1];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[5];
            score[4] = p2ScoreDisplay[6];
            break;
        case 6:
            count = 6;
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[1];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[4];
            score[4] = p2ScoreDisplay[5];
            score[5] = p2ScoreDisplay[6];
            break;
        case 7:
            count = 3;
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[2];
            score[2] = p2ScoreDisplay[5];
            break;
        case 8:
            count = 7;
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[1];
            score[2] = p2ScoreDisplay[2];
            score[3] = p2ScoreDisplay[3];
            score[4] = p2ScoreDisplay[4];
            score[5] = p2ScoreDisplay[5];
            score[6] = p2ScoreDisplay[6];
            break;
        case 9:
            count = 6;
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[1];
            score[2] = p2ScoreDisplay[2];
            score[3] = p2ScoreDisplay[3];
            score[4] = p2ScoreDisplay[5];
            score[5] = p2ScoreDisplay[6];
            break;
        default:
            goto winlabel;
    }

    SDL_RenderFillRects(game->renderer, score, count);
    free(score);
    score=NULL;
    count = 0;
return;

winlabel:
    free(score);
    score=NULL;
    game->gameover = TRUE;
    return;
}


void RenderText(void) {
    SDL_Texture *textTexture = NULL;
    if(game->gameover) {
        if(game->p1Score == 10) {
            textTexture = SDL_CreateTextureFromSurface(game->renderer, WinSurf);
            SDL_RenderCopy(game->renderer, textTexture, NULL, &(SDL_Rect){12+game->w/2 - WinSurf->w/2,
                                                                          game->h/2 - WinSurf->h/2,
                                                                          WinSurf->w, WinSurf->h});
            SDL_DestroyTexture(textTexture);
            textTexture = NULL;
        } else if (game->p2Score == 10) {
            textTexture = SDL_CreateTextureFromSurface(game->renderer, LoseSurf);
            SDL_RenderCopy(game->renderer, textTexture, NULL, &(SDL_Rect){game->w/2 - LoseSurf->w/2,
                                                                          game->h/2 - LoseSurf->h/2,
                                                                          LoseSurf->w, LoseSurf->h});
            SDL_DestroyTexture(textTexture);
            textTexture = NULL;
        }
        textTexture = SDL_CreateTextureFromSurface(game->renderer, ResetSurf);
        SDL_RenderCopy(game->renderer, textTexture, NULL, &(SDL_Rect){game->w/2 - ResetSurf->w/2,
                                                                          game->h/2 - ResetSurf->h/2 + 100,
                                                                          ResetSurf->w, ResetSurf->h});
        SDL_DestroyTexture(textTexture);
        textTexture = NULL;

    }

    textTexture = SDL_CreateTextureFromSurface(game->renderer, p1Surf);
    if(!textTexture) {
        printf_s("Texture creation failed: %s\n", TTF_GetError());
        return;
    }
    SDL_RenderCopy(game->renderer, textTexture, NULL, &(SDL_Rect){25, game->h - 45, p1Surf->w, p1Surf->h});
    
    SDL_DestroyTexture(textTexture);
    textTexture = NULL;

    if(!game->p2IsAI) {
        textTexture = SDL_CreateTextureFromSurface(game->renderer, p2Surf);
        if(!textTexture) {
            printf_s("Texture creation failed: %s\n", TTF_GetError());
            return;
        }
        SDL_RenderCopy(game->renderer, textTexture, NULL, &(SDL_Rect){game->w - p2Surf->w - 25, game->h - 45, p2Surf->w, p2Surf->h});
        
        SDL_DestroyTexture(textTexture);
        textTexture = NULL;
    }

    textTexture = SDL_CreateTextureFromSurface(game->renderer, ToggleAISurf);
    if(!textTexture) {
        printf_s("Texture creation failed: %s\n", TTF_GetError());
        return;
    }
    SDL_RenderCopy(game->renderer, textTexture, NULL, &(SDL_Rect){game->w/2 - ToggleAISurf->w/2, game->h-45, ToggleAISurf->w, ToggleAISurf->h});
    
    SDL_DestroyTexture(textTexture);
    textTexture = NULL;

}


void DestroyGameWindow(void) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
    free(game);
}