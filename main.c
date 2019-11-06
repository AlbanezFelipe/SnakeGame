#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>

static const float scale = 2.5f;
static const int width = 256 * 2.5f;
static const int height = 256 * 2.5f;

bool keyArrowUp;
bool keyArrowDown;
bool keyArrowRight;
bool keyArrowLeft;

struct coord {
    int x;
    int y;
};

int foodEaten = 0;

int magicNumber = 16;

struct coord snake[256];

struct coord food = {0, 0};

enum move {
    up,
    down,
    left,
    right
};

void foodAte(void);
void refreshFood(void);

void draw(SDL_Renderer * renderer);

bool wasFoodEaten(const SDL_Rect * snakeHead, const SDL_Rect * food);

void snakeGrow(void);

void moveSnake(enum move m);

struct coord lastCoord;

enum move lastMove = right;
void reset(void);

void lineCut(SDL_Renderer *r){
    int frameCut = magicNumber * scale;

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    for(int counter = 0;counter < width;counter += frameCut){
        SDL_RenderDrawLine(r, counter,0,counter, height);

    }
    for(int counter = 0;counter < height;counter += frameCut){
        SDL_RenderDrawLine(r, 0,counter,width, counter);
    }

}

int main(int argc, char **argv)
{

    srand(time(NULL));   // Random Number

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create a SDL window
    SDL_Window *window = SDL_CreateWindow("Hello, SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

    // Create a renderer (accelerated and in sync with the display refresh rate)
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    bool running = true;
    SDL_Event event;

    snake[0].x = 0;
    snake[0].y = 0;
    refreshFood();

    while(running)
    {
        // Process events
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                running = false;
            }

            if(event.type == SDL_KEYDOWN){
            keyArrowUp    = event.key.keysym.sym == SDLK_UP;
            keyArrowDown  = event.key.keysym.sym == SDLK_DOWN;
            keyArrowRight = event.key.keysym.sym == SDLK_RIGHT;
            keyArrowLeft  = event.key.keysym.sym == SDLK_LEFT;

            } else{
                keyArrowUp    = false;
                keyArrowDown  = false;
                keyArrowRight = false;
                keyArrowLeft  = false;
            }
        }

        SDL_Delay(60);

        // Clear screen
        SDL_RenderClear(renderer);

        //printf("X: %d\n", snake[0].x);
        //printf("Y: %d\n", snake[0].y);


        lastCoord.x = snake[0].x;
        lastCoord.y = snake[0].y;

        bool moved = false;

        if(keyArrowUp && lastMove != down){
            moveSnake(up);
            moved = true;
            lastMove = up;
        }
        if(keyArrowDown && lastMove != up){
            moveSnake(down);
            moved = true;
            lastMove = down;
        }
        if(keyArrowLeft && lastMove != right){
            moveSnake(left);
            moved = true;
            lastMove = left;
        }
        if(keyArrowRight && lastMove != left){
            moveSnake(right);
            moved = true;
            lastMove = right;
        }
        if(!moved){
            moveSnake(lastMove);
        }



//        SDL_RenderFillRect(renderer, &r);
        draw(renderer);

        //SDL_RenderDrawRect(renderer, &r);
        //lineCut(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // Show what was drawn
        SDL_RenderPresent(renderer);
    }

    // Release resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void foodAte(void){


}

void refreshFood(void){
    food.x = rand() % 16;
    food.y = rand() % 16;
}

void draw(SDL_Renderer * renderer){

    SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);

    //printf("Food x %d", food.x);
    //printf("Food y %d", food.y);

    SDL_Rect rFood = {
        food.x * magicNumber * scale,
        food.y * magicNumber * scale,
        magicNumber * scale,
        magicNumber * scale
    };

    SDL_RenderFillRect(renderer, &rFood);

    for(int i=0; i < foodEaten + 1; i++){

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect r = {
            snake[i].x * magicNumber * scale,
            snake[i].y * magicNumber * scale,
            magicNumber * scale,
            magicNumber * scale
        };

        if(i > 0){
        SDL_Rect a = {
            snake[0].x * magicNumber * scale,
            snake[0].y * magicNumber * scale,
            magicNumber * scale,
            magicNumber * scale
        };
            if(wasFoodEaten(&r, &a)){
                reset();
            }
        }

        SDL_RenderFillRect(renderer, &r);

    }

    SDL_Rect r = {
            snake[0].x * magicNumber * scale,
            snake[0].y * magicNumber * scale,
            magicNumber * scale,
            magicNumber * scale
        };

    if(wasFoodEaten(&rFood, &r)){
            refreshFood();
            foodEaten++;
            snakeGrow();
    }
}

bool wasFoodEaten(const SDL_Rect * snakeHead, const SDL_Rect * food){
    return SDL_HasIntersection(snakeHead, food);
}

void snakeGrow(void){
    snake[foodEaten].x = lastCoord.x;
    snake[foodEaten].y = lastCoord.y;
}

void moveSnake(enum move m){

    struct coord l;
    for(int i=0; i < foodEaten + 1; i++){

        if(i==0){


            l.x = snake[i].x;
            l.y = snake[i].y;

            if(m == up){
                snake[0].y -= 1;
            }
            if(m == down){
                snake[0].y += 1;
            }
            if(m == left){
                snake[0].x -= 1;
            }
            if(m == right){
                snake[0].x += 1;
            }

            if(snake[0].x < 0 || snake[0].x > magicNumber -1 || snake[0].y < 0 || snake[0].y > magicNumber -1){
            reset();
            }


        }else{
            struct coord c = {snake[i].x, snake[i].y};

            snake[i].x = l.x;
            snake[i].y = l.y;

            l.x = c.x;
            l.y = c.y;

        }
    }
}

void reset(void){
printf("Game Over; Score: %d\n", foodEaten);
snake[0].x = 0;
    snake[0].y = 0;
    refreshFood();
    foodEaten = 0;
    lastMove = right;

}

