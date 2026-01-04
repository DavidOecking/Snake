#include<stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>

#define EMPTTY 0
#define SNAKE 1
#define FOOD 2

typedef struct {
    int dimension;
    int *board;
} GameState;

typedef enum {
    UP, RIGHT, DOWN, LEFT
} Dir;


void update_game_state(){

};

void render(){

};

int main(int argc, char** argv){
    int dimension;
    printf("Enter Dimension: ");
    scanf("%d", &dimension);

    int *board = calloc(dimension * dimension, sizeof(int));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow(
        "Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480, SDL_WINDOW_SHOWN
    );
    
    Dir dir = UP;
    bool running = true;   

    Uint32 last_tick = SDL_GetTicks();
    Uint32 frame_time = 0;
    const Uint32 STEP_MS = 150;

    while(running){
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym){
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_UP: dir = UP; break;
                    case SDLK_RIGHT: dir = RIGHT; break;
                    case SDLK_DOWN: dir = DOWN; break;
                    case SDLK_LEFT: dir = LEFT; break;
                }
            }
        }


        Uint32 now = SDL_GetTicks();
        Uint32 loop_time = now - last_tick;
        last_tick = now;
        frame_time += loop_time;

        while(frame_time >= STEP_MS) {
            update_game_state();
            frame_time -= STEP_MS;
        }

        render();
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    free(board);
    return 0;
}


