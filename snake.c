#include<stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY 0
#define SNAKE 1
#define FOOD 2
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640

typedef enum {
    UP, RIGHT, DOWN, LEFT
} Dir;

typedef enum {
  WON, LOST, PAUSED, RUNNING
} State;

typedef struct {
    int dimension;
    int numOfFields;
    int *board;
    int *snake;
    int length;
    Dir dir;
    State state;
} GameState;

int randomPosition(int modulo){
    return rand() % modulo;
}

void placeNewFood(GameState* state){
    int position = randomPosition(state->numOfFields);
    for(int i = 0; i < state->numOfFields; i++){
        int newPosition = position+i%(state->numOfFields);
        if(state->board[newPosition] == EMPTY){
            state->board[newPosition] = FOOD;
            return;
        }
    }
}

void moveSnake(GameState* state, int newHead, bool food){
    for(int i = state->length-1; i >= 0; i--){
        state->snake[i+1] = state->snake[i]; 
        if(i == 0){
            state->snake[i] = newHead;
        }
    }

    if(food){
        state->length++;
        if(state->length == state->numOfFields){
            state->state = WON;
        }
    }else{
        state->board[state->snake[state->length]] = EMPTY;
        state->snake[state->length] = 0;
    }

    state->board[newHead] = SNAKE;
}

void update_game_state(GameState* state){

    int headPosition = state->snake[0];
    int newPosition;
    switch(state->dir){
        case UP: 
            if(headPosition/state->dimension == 0){
                state->state = LOST;
            }else{
                newPosition = headPosition - state->dimension;
            }
            break;
        case RIGHT:
            if(headPosition%state->dimension == (state->dimension-1)){
                state->state = LOST;
            }else{
                newPosition = headPosition + 1;
            }
            break;
        case DOWN:
            if(headPosition/state->dimension == (state->dimension-1)){
                state->state = LOST;
            }else{
                newPosition = headPosition + state->dimension;
            }
            break;
        case LEFT:
            if(headPosition%state->dimension == 0){
                state->state = LOST;
            }else{
                newPosition = headPosition - 1;
            }
            break;    
    }

    if(state->state == LOST){
        return;
    }

    int previousTail = state->snake[state->length-1];
    if(newPosition == previousTail){
        moveSnake(state, newPosition, false);
    }else{
        int newField = state->board[newPosition];

        switch(newField){
            case EMPTY:
                moveSnake(state, newPosition, false);
                break;
            case SNAKE:
                state->state = LOST;
                break;
            case FOOD:
                moveSnake(state, newPosition, true);
                placeNewFood(state);
                break;
        }
    }

}

void render(GameState* state, SDL_Renderer* gRenderer){
    SDL_SetRenderDrawColor( gRenderer, 0xb1, 0xab, 0xb3, 255 ); 
    SDL_RenderClear( gRenderer );

    int cellHeight = SCREEN_HEIGHT / state->dimension;
    int cellWidth = SCREEN_WIDTH / state->dimension; 

    for(int i = 0; i<state->numOfFields; i++){
        int column = i % state->dimension;
        int row = i / state->dimension;

        SDL_Rect fillRect = { cellWidth * column, cellHeight * row, cellWidth, cellHeight}; 
        switch(state->board[i]){
            case EMPTY:
                break;
            case SNAKE:
                SDL_SetRenderDrawColor( gRenderer, 0x17, 0x4f, 0x26, 255 ); 
                SDL_RenderFillRect( gRenderer, &fillRect );
                break;
            case FOOD:
                SDL_SetRenderDrawColor( gRenderer, 0x94, 0x27, 0x09, 255 ); 
                SDL_RenderFillRect( gRenderer, &fillRect );
                break;
        }
    }

    SDL_RenderPresent(gRenderer);
};

void renderAscii(GameState* gameState){
    for(int i = 0; i<gameState->numOfFields; i++){
        printf("%d ", gameState->board[i]);
        if(i % gameState->dimension == (gameState->dimension -1)){
            printf("\n");
        }
    }
    printf("\n");
};

/*
TODO
Add Pause and Victory Screen
Add Option to Restart Game
Make sure Game Start is possible
*/

int main(int argc, char** argv){
    srand((unsigned int)time(NULL));

    int dimension;
    printf("Enter Dimension: ");
    scanf("%d", &dimension);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow(
        "Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN
    );

    SDL_Renderer *gRenderer = SDL_CreateRenderer( win, -1, SDL_RENDERER_ACCELERATED );
    
    Dir dir = UP;
    bool running = true;   

    Uint32 last_tick = SDL_GetTicks();
    Uint32 frame_time = 0;
    const Uint32 STEP_MS = 150;

    GameState state;
    state.dimension = dimension;
    state.numOfFields = dimension * dimension;
    state.dir = LEFT;
    state.board = calloc(state.numOfFields, sizeof(int));
    state.snake = calloc(state.numOfFields, sizeof(int));
    int initialPosition = randomPosition(state.numOfFields);
    printf("InitialPositon: %d \n", initialPosition);
    state.snake[0] = initialPosition;
    state.length = 1;
    state.board[initialPosition] = SNAKE;
    placeNewFood(&state);
    state.state = RUNNING;

    while(running){
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym){
                    case SDLK_ESCAPE: 
                        if(state.state == PAUSED) {
                            state.state = RUNNING; 
                        }else if(state.state == RUNNING){
                            state.state = PAUSED; 
                        }
                        break;
                    case SDLK_UP: if(state.dir != DOWN && state.state == RUNNING) {state.dir = UP;} break;
                    case SDLK_RIGHT: if(state.dir != LEFT  && state.state == RUNNING) {state.dir = RIGHT;} break;
                    case SDLK_DOWN: if(state.dir != UP  && state.state == RUNNING) {state.dir = DOWN;} break;
                    case SDLK_LEFT: if(state.dir != RIGHT  && state.state == RUNNING) {state.dir = LEFT;} break;
                }
            }
        }

        Uint32 now = SDL_GetTicks();

        if(state.state != RUNNING){
            last_tick = now;
            frame_time = 0;
            continue;
        }

        Uint32 loop_time = now - last_tick;
        last_tick = now;
        frame_time += loop_time;

        while(frame_time >= STEP_MS) {
            update_game_state(&state);
            frame_time -= STEP_MS;
            if(state.state != RUNNING){
                if(state.state == WON){
                    printf("Game won.");
                }
                if(state.state == LOST){
                    printf("Game lost. Score: %d.", state.length);
                }
                break;
            }
        }
        render(&state, gRenderer);
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    free(state.board);
    free(state.snake);
    return 0;
}


