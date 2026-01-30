/* 
    FILE: main.cpp

    DESCRIPTION:

    This is just a current example to try and make a box appear using SDL

*/

// Standard Libraries
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#define WINDOW_WIDTH    600
#define WINDOW_HEIGHT   600

// SDL3 Includes
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// Colour variables
static uint8_t r = 255;
static uint8_t g = 255;
static uint8_t b = 255;

class Circles
{
    public:
    float circle_x;
    float circle_y;
    float circle_radius;

    // Circle Initialise
    Circles() 
    {
        circle_x = 300.0f;
        circle_y = 300.0f;
        circle_radius = 40.0f;
    }
    

    void DrawCircle(SDL_Renderer *r, int cx, int cy, int radius)
    {
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x*x + y*y <= radius*radius) {
                    SDL_RenderPoint(r, cx + x, cy + y);
                }
            }
        }
    }
};

class Rectangles
{
    public:
    SDL_FRect myRect;

    // Rectangle Dimensions
    Rectangles()
    {
        myRect = {200, 10, 50, 50};  // x, y, width, height
    }
};

// Define Shape Classes
static Rectangles* box = NULL;
static Circles* ball = NULL;

// Change Object Colour
void randomiseColour() 
{
    r = rand() % 256;  // Random value 0-255
    g = rand() % 256;
    b = rand() % 256;
}

// Rectangle Movement and Collisions
class ObjectMovement
{
    public:
    // Object Movement
    float vel_X;  
    float vel_Y;
    float gravity;

    float* x_pos;  
    float* y_pos;
    float width;   
    float height;

    ObjectMovement(float* x, float* y, float w, float h) {
        x_pos = x;     // Save the address
        y_pos = y;
        width = w;
        height = h;

        // Initialise velocities
        srand(time(NULL));
        vel_X = (rand() % 1001) - 500;
        vel_Y = 0.0f;
        gravity = 98.1f;
        randomiseColour();
    }

    // Updates the position
    void update(float deltaTime) 
    {
        // Move
        *x_pos += vel_X * deltaTime;
        
        vel_Y += gravity * deltaTime;
        *y_pos += vel_Y * deltaTime;
        
        // Collision with left wall
        if (*x_pos < 0) {
            *x_pos = 0;
            vel_X = -vel_X;
            randomiseColour();
        }
        
        // Collision with right wall
        if (*x_pos + width > WINDOW_WIDTH) {
            *x_pos = WINDOW_WIDTH - width;
            vel_X = -vel_X;
            randomiseColour();
        }
        
        // Bounce off edges - TOP and BOTTOM

        // Top
        if (*y_pos< 0) 
        {
            *y_pos = height;      // Push back inside
            vel_Y = -vel_Y * 0.8f;
            randomiseColour();
        }

        // Bottom
        if (*y_pos + height > WINDOW_HEIGHT) 
        {
            *y_pos = WINDOW_HEIGHT - height;  // Push back inside
            vel_Y = -vel_Y * 0.8f;
            randomiseColour();
        }
    }

};



/* We use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static ObjectMovement* boxMove = NULL;
static ObjectMovement* ballMove = NULL;

static uint64_t lastTime = 0;


/* 
    Initilisation Phase

    CLASS: SDL_AppResult

    FUNCTIONS: 
    - SDL_AppInit
    - SDL_AppEvent
*/

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Example Renderer Rectangles", "1.0", "com.example.renderer-rectangles");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/rectangles", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    ball = new Circles();               // Create the ball
    box =  new Rectangles();            // Creates the box

    boxMove = new ObjectMovement(&box->myRect.x, &box->myRect.y, 
                            box->myRect.w, box->myRect.h);

    ballMove = new ObjectMovement(&ball->circle_x, 
                            &ball->circle_y,
                            ball->circle_radius, 
                            ball->circle_radius);

    // Previous Time 
    lastTime = SDL_GetTicks();

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}


/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* 
    Main Function Loop

    CLASSES: SDL_AppIterate

    FUNCTIONS:
    - SDL_AppIterate

*/
SDL_AppResult SDL_AppIterate (void *appstate)
{
    // Time-keeping
    uint64_t now = SDL_GetTicks();
    float deltaTime = (now - lastTime) / 1000.0f;  
    lastTime = now;
    
    // Calling update function
    boxMove->update(deltaTime);
    ballMove->update(deltaTime);

    // Clear and draw
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderRect(renderer, &box->myRect);

    SDL_SetRenderDrawColor(renderer, 255, 100, 100, SDL_ALPHA_OPAQUE);
    ball->DrawCircle(renderer, ball->circle_x, ball->circle_y, ball->circle_radius);
    
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // Clean up allocated memory
    if (ball) {
        delete ball;
        ball = NULL;
    }
    if (box) {
        delete box;
        box = NULL;
    }
    if (ballMove) {
        delete ballMove;
        ballMove = NULL;
    }
    if (boxMove) {
        delete boxMove;
        boxMove = NULL;
    }
    /* SDL will clean up the window/renderer for us. */
}
