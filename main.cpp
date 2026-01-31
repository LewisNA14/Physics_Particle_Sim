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

// App Window Screen size
#define WINDOW_WIDTH    600
#define WINDOW_HEIGHT   600

// SDL3 Includes
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// Colour variables
static uint8_t r = 255;
static uint8_t g = 255;
static uint8_t b = 255;

/* =======================================================================
   Object Classes
==========================================================================*/

/* Circle Object Class */
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
        for (int y = -radius; y <= radius; y++) 
        {
            for (int x = -radius; x <= radius; x++) 
            {
                if (x*x + y*y <= radius*radius) 
                {
                    SDL_RenderPoint(r, (float)(cx + x), (float)(cy + y));
                }
            }
        }
    }
};

/* Rectangle Object Class */
class Rectangles
{
public:
    float rect_x;
    float rect_y;
    float rect_w;
    float rect_h;

    Rectangles()
    {
        rect_x = 225;
        rect_y = 35; 
        rect_w = 25;
        rect_h = 25;
    }
    
    void DrawRectangle(SDL_Renderer *r, int rx, int ry, int rw, int rh)
    {
        for (int y = -rh; y <= rh; y++)
        {
            for(int x = -rw; x <= rw; x++)
            {
                SDL_RenderPoint(r, (float)(rx + x), (float)(ry + y));         
            }
        }
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
        if (*x_pos - width < 0.0) 
        {
            *x_pos = 0 + width;
            vel_X = -vel_X;
        }
        
        // Collision with right wall
        if (*x_pos + width > WINDOW_WIDTH) 
        {
            *x_pos = WINDOW_WIDTH - width;
            vel_X = -vel_X;
        }
        
        // Bounce off edges - TOP and BOTTOM

        // Top
        if (*y_pos< 0) 
        {
            *y_pos = height;      // Push back inside
            vel_Y = -vel_Y * 0.8f;
        }

        // Bottom
        if (*y_pos + height > WINDOW_HEIGHT) 
        {
            *y_pos = WINDOW_HEIGHT - height;  // Push back inside
            vel_Y = -vel_Y * 0.8f;
        }
    }

    // void Collision(bool col_x, bool col_y)
    // {
    //     if (col_y == true)
    //     {
    //         vel_Y = -vel_Y;
    //     }
    //     else if (col_x)
    //     {
    //         vel_X = -vel_X;
    //     }
    // }

};

/* We use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static ObjectMovement* boxMove = NULL;
static ObjectMovement* ballMove = NULL;

static uint64_t lastTime = 0;

static bool collision_x = false;
static bool collision_y = false;

/* =======================================================================
Function: SDL_AppInit

Description: Initialises all of the variables needed in the main loop
==========================================================================*/

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

    box =  new Rectangles();            // Creates the box
    ball = new Circles();               // Create the ball
    
    boxMove = new ObjectMovement(&box->rect_x, 
                            &box->rect_y, 
                            box->rect_w, 
                            box->rect_h);

    ballMove = new ObjectMovement(&ball->circle_x, 
                            &ball->circle_y,
                            ball->circle_radius, 
                            ball->circle_radius);

    // Previous Time 
    lastTime = SDL_GetTicks();

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* =======================================================================
Function: SDL_AppEvent

Parameters: appstate, event

Description: This function runs when a new event (mouse input, keypresses, etc) occurs. 
==========================================================================*/

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* =======================================================================
Function: SDL_AppIterate

Description: Acts as the main iterative loop of code for the system.
    It renders the objects as well as calls the movement update function etc.
==========================================================================*/
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
    box->DrawRectangle(renderer, box->rect_x, box->rect_y, box->rect_w, box->rect_h);

    SDL_SetRenderDrawColor(renderer, 255, 100, 100, SDL_ALPHA_OPAQUE);
    ball->DrawCircle(renderer, ball->circle_x, ball->circle_y, ball->circle_radius);

    // Checking Collisions
    /* if (((box->rect_x + box->rect_w) >= (ball->circle_x - ball->circle_radius)) || 
    ((box->rect_x - box->rect_w) >= (ball->circle_x + ball->circle_radius)))
    {
        collision_x = true;
        boxMove->Collision(collision_x, collision_y);
        ballMove->Collision(collision_x, collision_y);
        collision_x = false;
    }

    if (((box->rect_y + box->rect_w) >= (ball->circle_y - ball->circle_radius)) ||
    ((box->rect_x - box->rect_w) >= (ball->circle_x + ball->circle_radius)))
    {
        collision_y = true;
        boxMove->Collision(collision_x, collision_y);
        ballMove->Collision(collision_x, collision_y);
        collision_y = false;
    } */
    
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/* =======================================================================
Function: SDL_AppQuit

Description: Performs the garbage collection for the process, cleaning up
allocated memory
==========================================================================*/
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
