#include "game.h"
#include "scene/title_scene.h"

/*
Menu Scene Text 
*/
const char *menuTitleString = "KayoBreaker";
const char *menuPlayString = "Start";
const char *menuQuitString = "Quit";

typedef struct TitleScene {
    SDL_Surface *titleSurf;
    SDL_Surface *playSurf;
    SDL_Surface *quitSurf;

    i32 selector;
} TitleScene;


void UpdateTitleScene(f32 deltaTime, void *scene);
void RenderTitleScene(void *scene);


b8 InitializeTitleScene(Scene *outScene) {
    if(SCENE_TYPE_NONE != outScene->sceneType || NULL != outScene->internalData) {
        printf_s("This scene has already been initialized, please destroy the scene before reinitializing it.\n");
        return FALSE;
    }

    outScene->sceneType = SCENE_TYPE_MENU;
    outScene->internalData = SDL_malloc(sizeof(TitleScene));
    if(NULL == outScene->internalData) {
        printf_s("Failed to allocate nessacary memory for Game Scene.\n");
        return FALSE;
    }
    
    SDL_memset(outScene->internalData, 0, sizeof(TitleScene));
    TitleScene* titleData = (TitleScene*)outScene->internalData;

    SDL_Color white = {255, 255, 255, 255};

    titleData->titleSurf = TTF_RenderText_Blended(GetGame()->font, menuTitleString, white);
    if(!titleData->titleSurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    titleData->playSurf = TTF_RenderText_Blended(GetGame()->font, menuPlayString, white);
    if(!titleData->playSurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    titleData->quitSurf = TTF_RenderText_Blended(GetGame()->font, menuQuitString, white);
    if(!titleData->quitSurf) {
        printf_s("Surface creation failed: %s\n", TTF_GetError());
        return FALSE;
    }

    titleData->selector = 0;

    outScene->Update = UpdateTitleScene;
    outScene->Render = RenderTitleScene;
    outScene->isActive = TRUE;
    outScene->isRendered = TRUE;

    return TRUE;
}

void UpdateTitleScene(f32 deltaTime, void *scene) {
    Scene *titleScene = (Scene *)scene;
    if(SCENE_TYPE_MENU != titleScene->sceneType) {
        printf_s("unexpected scene type.");
        return;
    }
    if(!titleScene->isActive) {
        return;
    }
    TitleScene* titleData = (TitleScene*)titleScene->internalData;
    if(GetKeyPressed(SDLK_w)) {
        if (titleData->selector == 1) {
            titleData->selector = 0;
        } else {
            titleData->selector = 1;
        }
    }
    if(GetKeyPressed(SDLK_s)) {
        if (titleData->selector == 1) {
            titleData->selector = 0;
        } else {
            titleData->selector = 1;
        }
    }
    if(GetKeyPressed(SDLK_RETURN)) {
        if(titleData->selector == 0) {
            titleScene->isActive = FALSE;
            titleScene->isRendered = FALSE;
            GetGame()->activeScene->isActive = TRUE;
            GetGame()->activeScene->isRendered = TRUE;
        } else {
            GetGame()->isRunning = FALSE;
        }
    }
    
}

void RenderTitleScene(void *scene) {
    Scene *titleScene = (Scene *)scene;
    if(SCENE_TYPE_MENU != titleScene->sceneType) {
        printf_s("unexpected scene type.");
        return;
    }
    if(!titleScene->isRendered) {
        return;
    }
    TitleScene* titleData = (TitleScene*)titleScene->internalData;

    SDL_SetRenderDrawColor(GetGame()->renderer, 255, 20, 20, 128);
    if(0 == titleData->selector) {
        SDL_RenderFillRect(GetGame()->renderer, &(SDL_Rect){(GetGame()->w / 2) - (titleData->playSurf->w / 2) - 1,
                                                                (GetGame()->h - GetGame()->h/4) - (titleData->playSurf->h) - 1,
                                                                titleData->playSurf->w + 1, titleData->playSurf->h + 1});
    } else {
        SDL_RenderFillRect(GetGame()->renderer, &(SDL_Rect){(GetGame()->w / 2) - (titleData->playSurf->w / 2) - 1,
                                                                (GetGame()->h - GetGame()->h/4) - (titleData->quitSurf->h) + (titleData->playSurf->h + 20) - 1,
                                                                titleData->quitSurf->w + 1, titleData->quitSurf->h + 1});
    }

    SDL_Texture *text = NULL;
    text = SDL_CreateTextureFromSurface(GetGame()->renderer, titleData->titleSurf);
    SDL_RenderCopy(GetGame()->renderer, text, NULL, &(SDL_Rect){(GetGame()->w / 2) - (titleData->titleSurf->w), (GetGame()->h / 4),
                                                                titleData->titleSurf->w*2, titleData->titleSurf->h*2});
    SDL_DestroyTexture(text);
    text = NULL;

    text = SDL_CreateTextureFromSurface(GetGame()->renderer, titleData->playSurf);
    SDL_RenderCopy(GetGame()->renderer, text, NULL, &(SDL_Rect){(GetGame()->w / 2) - (titleData->playSurf->w / 2),
                                                                (GetGame()->h - GetGame()->h/4) - (titleData->playSurf->h),
                                                                titleData->playSurf->w, titleData->playSurf->h});
    SDL_DestroyTexture(text);
    text = NULL;

    text = SDL_CreateTextureFromSurface(GetGame()->renderer, titleData->quitSurf);
    SDL_RenderCopy(GetGame()->renderer, text, NULL, &(SDL_Rect){(GetGame()->w / 2) - (titleData->playSurf->w / 2),
                                                                (GetGame()->h - GetGame()->h/4) - (titleData->quitSurf->h) + (titleData->playSurf->h + 20),
                                                                titleData->quitSurf->w, titleData->quitSurf->h});
    SDL_DestroyTexture(text);
    text = NULL;
}

void DestroyTitleScene(Scene *scene) {
    if(SCENE_TYPE_MENU != scene->sceneType) {
        printf_s("unexpected scene type.");
        return;
    }
    TitleScene* titleData = (TitleScene*)scene->internalData;

    SDL_FreeSurface(titleData->quitSurf);
    SDL_FreeSurface(titleData->playSurf);
    SDL_FreeSurface(titleData->titleSurf);

    SDL_free(scene->internalData);
    scene->internalData = NULL;
}