/*============================================================================ 
*    FILE: main.cpp
*
*    DESCRIPTION:
*
*    This is just a current example to try and make a box appear using SDL
*
*=============================================================================*/

// Standard Libraries
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

// File Includes
#include "physics.h" 

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

// App Window Screen size
#define WINDOW_WIDTH    600
#define WINDOW_HEIGHT   600

// SDL3 Includes
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/*=======================================================================
*   SECTION: Shape Drawing
* =======================================================================*/

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

/*=======================================================================
*  SECTION: Global Variables
* =======================================================================*/
    
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static ObjectMovement* boxMove = NULL;
static ObjectMovement* ballMove = NULL;

static Collisions* collisionSystem = NULL;

static uint64_t lastTime = 0;

//----------------------------------------------------------------------------------------------------------------------
//      SDL Section


/*  =======================================================================
    Function: SDL_AppInit

    Description: Initialises all of the variables needed in the main loop
    ======================================================================== */

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Example Renderer Rectangles", "1.0", "com.example.renderer-rectangles");

    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/rectangles", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) 
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    box =  new Rectangles();            // Creates the box
    ball = new Circles();               // Create the ball
    
    boxMove  = new ObjectMovement(&box->rect_x, 
                            &box->rect_y, 
                            box->rect_w, 
                            box->rect_h);

    ballMove = new ObjectMovement(&ball->circle_x, 
                            &ball->circle_y,
                            ball->circle_radius, 
                            ball->circle_radius);

    collisionSystem = new Collisions();
    // Previous Time 
    lastTime = SDL_GetTicks();

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/*  =======================================================================
    Function: SDL_AppEvent

    Parameters: appstate, event

    Description: 
    
    This function runs when a new event (mouse input, keypresses, etc) occurs. 
    ======================================================================== */

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) 
    {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* =======================================================================
    Function: SDL_AppIterate

    Description: 
    
    Acts as the main iterative loop of code for the system.
    It renders the objects as well as calls the movement update function etc.
    ========================================================================== */
SDL_AppResult SDL_AppIterate (void *appstate)
{
    
    // Time-keeping
    uint64_t now = SDL_GetTicks();
    float deltaTime = (now - lastTime) / 1000.0f;  
    lastTime = now;
    
    // Update physics first
    boxMove->updatePos(deltaTime);
    ballMove->updatePos(deltaTime);
    
    // Then update contact points
    boxMove->updateContactPoints();
    ballMove->updateContactPoints();

    // Collision Detection / Handling
    collisionSystem->CheckBorderCollision(boxMove);
    collisionSystem->CheckBorderCollision(ballMove);
    collisionSystem->CheckObjectCollision(boxMove, ballMove);

    // Clear and draw
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    DrawRectangle(renderer, box->rect_x, box->rect_y, box->rect_w, box->rect_h);

    SDL_SetRenderDrawColor(renderer, 255, 100, 100, SDL_ALPHA_OPAQUE);
    DrawCircle(renderer, ball->circle_x, ball->circle_y, ball->circle_radius);

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/*  =======================================================================
    Function: SDL_AppQuit

    Description: 
    Performs the garbage collection for the process, cleaning up
    allocated memory
    ========================================================================== */
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
