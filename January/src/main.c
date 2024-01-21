#include <Windows.h>
#include <stdio.h>
#include <memory.h>
#include "defines.h"
#include "SDL.h"


SDL_Color white = {255, 255, 255, 255};

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
    SDL_Window* window;
    SDL_Renderer* renderer;
    b8 isRunning;

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

#define SPEED_CAP 500
#define FLOAT_TOLERANCE 0.0001f

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

void DestroyGameWindow(void);

int main(int argc, char** argv) {
    
    if(!InitializeGameWindow()) {
        return -1;
    }

    u64 lastFrameTime = 0;
    u64 frameStartTime = 0;
    u64 frameEndTime = 0;
    u64 frameTime = 0;
    u64 targetFPS = 30;
    u64 frameDelay = 1000 / targetFPS;
    f64 deltaTime = 0.0;

    Setup();

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

    game->name = "Pong-Like";

    SDL_Init(SDL_INIT_EVERYTHING);

    game->window = SDL_CreateWindow(game->name, game->x, game->y, game->w, game->h, SDL_WINDOW_SHOWN);
    if(!game->window) {
        return FALSE;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, 0);
    if(!game->renderer) {
        return FALSE;
    }

    game->isRunning = TRUE;

    return TRUE;
}

void Setup(void) {

    game->ball.x = game->w / 2;
    game->ball.y = game->h / 2;

    game->ball.w = 15; 
    game->ball.h = 15;

    game->ball.speed_h = 100;
    game->ball.speed_v = 100;

    game->player1.x = 50;
    game->player1.y = game->h/2;

    game->player1.w = 15;
    game->player1.h = 63;

    game->player1.speed_v = 0;
    game->p1Score = 0;

    game->player2.x = game->w - 50;
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
                printf_s("'I' released");
                game->p2IsAI = !game->p2IsAI;
                Setup();
            }
            if(ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_DOWN) {
                if(!game->p2IsAI) {
                    game->player2.speed_v = 0;
                }
            }
            break;
        case SDL_KEYDOWN:
            if(ev.key.keysym.sym == SDLK_w) {
                game->player1.speed_v = -150;
            }
            if(ev.key.keysym.sym == SDLK_s) {
                game->player1.speed_v = 150;
            }
            if(ev.key.keysym.sym == SDLK_UP) {
                if(!game->p2IsAI) {
                    game->player2.speed_v = -150;
                }
            }
            if(ev.key.keysym.sym == SDLK_DOWN) {
                if(!game->p2IsAI) {
                    game->player2.speed_v = 150;
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
    UpdateBall(deltaTime);
    UpdatePaddles(deltaTime);
    UpdateScore();

}
void UpdateBall(f64 deltaTime) {
    if((game->ball.x+game->ball.w > 100) && ((game->ball.h) < game->w-100)) {
        if((((game->ball.y + game->ball.h) > (game->h-50)) && game->ball.speed_v > 0) || ((game->ball.y < 50) && game->ball.speed_v < 0)) {
            game->ball.speed_v*=-1.00f;
        }
    }

        if((game->ball.x < (game->player1.x + game->player1.w)) && (game->ball.x > game->player1.x)) {
        if(((game->ball.y+game->ball.h) < (game->player1.y + game->player1.h)) && (game->ball.y > game->player1.y)) {
            if(game->ball.speed_h < 0) {
                game->ball.speed_h *= -1.25f;
                game->ball.speed_v += game->player1.speed_v;
                return;
            }
        }
    }

    if(((game->ball.x+game->ball.w) < (game->player2.x + game->player2.w)) && ((game->ball.x + game->ball.w) > game->player2.x)) {
        if(((game->ball.y+game->ball.h) < (game->player2.y + game->player2.h)) && (game->ball.y > game->player2.y)) {
            if(game->ball.speed_h > 0) {
                game->ball.speed_h *= -1.25f;
                game->ball.speed_v += game->player2.speed_v;
                return;
            }
        }
    }
    
    game->ball.x += game->ball.speed_h * deltaTime;
    game->ball.y += game->ball.speed_v * deltaTime;
}

void UpdatePaddles(f64 deltaTime) {

    game->player1.y += game->player1.speed_v * deltaTime;
    if(game->player1.y < 0) {
        game->player1.y = 0;
    }
    if(game->player1.y > game->h-game->player1.h) {
        game->player1.y = (game->h-game->player1.h);
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
    if(game->player2.y < 0) {
        game->player2.y = 0;
    }
    if(game->player2.y > game->h-game->player2.h) {
        game->player2.y = (game->h-game->player2.h);
    }
}

void UpdateScore(void) {
    if(game->ball.x > game->w) {
        game->p1Score++;
        game->ball.x = game->w/2;
        game->ball.y = game->h/2;
        game->ball.speed_h = -100;
        game->ball.speed_v = -100;
    }

    if(game->ball.x + game->ball.w < 0) {
        game->p2Score++;
        game->ball.x = game->w/2;
        game->ball.y = game->h/2;
        game->ball.speed_h = 100;
        game->ball.speed_v = 100;
    }
}

void Render(void) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 20, 255);
    SDL_RenderClear(game->renderer);
    SDL_SetRenderDrawColor(game->renderer, white.r, white.g, white.b, white.a);

    RenderBoard();
    if(game->gameover) {
        // TODO: Display "Game Over!" or "You Win!" based on player score. "Press R to Reset Game."
        SDL_RenderPresent(game->renderer);
        return;
    };
    RenderBall();
    RenderPaddles();
    RenderScore();

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
    u32 p2ScoreHPos = game->w - (game->w/4 - 32);
    u32 count = 0;
    SDL_Rect *score = NULL;

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
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[1];
            score[2] = p1ScoreDisplay[2];
            score[3] = p1ScoreDisplay[4];
            score[4] = p1ScoreDisplay[5];
            score[5] = p1ScoreDisplay[6];
            break;
        case 1:
            count = 2;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p1ScoreDisplay[2];
            score[1] = p1ScoreDisplay[5];
            break;
        case 2:
            count = 5;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[2];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[4];
            score[4] = p1ScoreDisplay[6];
            break;
        case 3:
            count = 5;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[2];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[5];
            score[4] = p1ScoreDisplay[6];
            break;
        case 4:
            count = 4;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p1ScoreDisplay[1];
            score[1] = p1ScoreDisplay[2];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[5];
            break;
        case 5:
            count = 5;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[1];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[5];
            score[4] = p1ScoreDisplay[6];
            break;
        case 6:
            count = 6;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[1];
            score[2] = p1ScoreDisplay[3];
            score[3] = p1ScoreDisplay[4];
            score[4] = p1ScoreDisplay[5];
            score[5] = p1ScoreDisplay[6];
            break;
        case 7:
            count = 3;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p1ScoreDisplay[0];
            score[1] = p1ScoreDisplay[2];
            score[2] = p1ScoreDisplay[5];
            break;
        case 8:
            count = 7;
            score = malloc(sizeof(SDL_Rect)*count);
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
            score = malloc(sizeof(SDL_Rect)*count);
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
    free(score);
    score = NULL;
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
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[1];
            score[2] = p2ScoreDisplay[2];
            score[3] = p2ScoreDisplay[4];
            score[4] = p2ScoreDisplay[5];
            score[5] = p2ScoreDisplay[6];
            break;
        case 1:
            count = 2;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p2ScoreDisplay[2];
            score[1] = p2ScoreDisplay[5];
            break;
        case 2:
            count = 5;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[2];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[4];
            score[4] = p2ScoreDisplay[6];
            break;
        case 3:
            count = 5;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[2];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[5];
            score[4] = p2ScoreDisplay[6];
            break;
        case 4:
            count = 4;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p2ScoreDisplay[1];
            score[1] = p2ScoreDisplay[2];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[5];
            break;
        case 5:
            count = 5;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[1];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[5];
            score[4] = p2ScoreDisplay[6];
            break;
        case 6:
            count = 6;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[1];
            score[2] = p2ScoreDisplay[3];
            score[3] = p2ScoreDisplay[4];
            score[4] = p2ScoreDisplay[5];
            score[5] = p2ScoreDisplay[6];
            break;
        case 7:
            count = 3;
            score = malloc(sizeof(SDL_Rect)*count);
            score[0] = p2ScoreDisplay[0];
            score[1] = p2ScoreDisplay[2];
            score[2] = p2ScoreDisplay[5];
            break;
        case 8:
            count = 7;
            score = malloc(sizeof(SDL_Rect)*count);
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
            score = malloc(sizeof(SDL_Rect)*count);
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
    game->gameover = TRUE;
    return;
}


void DestroyGameWindow(void) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
    free(game);

}