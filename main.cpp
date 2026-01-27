/* 
    FILE: main.ccp

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

// Rectangle Movement and Collisions
class ObjectMovement
{
public:
    SDL_FRect myRect;
    float vel_X;  // Store velocity in the class!
    float vel_Y;
    bool Collision = false;
    
    uint8_t r, g, b;  // RGB color values


    
    ObjectMovement() {
        myRect = {100, 100, 50, 50};
        myRect.x = 100;
        myRect.y = 300;

        vel_X = 0.0f;  // 100 pixels per second
        vel_Y = 0.0f;
        
        randomizeColor();
    }

    void randomizeColor() 
    {
        r = rand() % 256;  // Random value 0-255
        g = rand() % 256;
        b = rand() % 256;
    }
    
    // Add a method to update position
    void update(float deltaTime) {
        // Move
        myRect.x += vel_X * deltaTime;
        myRect.y += vel_Y * deltaTime;
        
        // Bounce off edges - LEFT and RIGHT
        if (myRect.x < 0) 
        {
            myRect.x = 0;                       // Push back inside
            vel_X = -vel_X;
            Collision = true;
        }
        if (myRect.x + myRect.w > WINDOW_WIDTH) 
        {
            myRect.x = WINDOW_WIDTH - myRect.w;  // Push back inside
            vel_X = -vel_X;
            Collision = true;
        }
        
        // Bounce off edges - TOP and BOTTOM
        if (myRect.y < 0) 
        {
            myRect.y = 0;  // Push back inside
            vel_Y = -vel_Y;
            Collision = true;
        }
        if (myRect.y + myRect.h > WINDOW_HEIGHT) 
        {
            myRect.y = WINDOW_HEIGHT - myRect.h;  // Push back inside
            vel_Y = -vel_Y;
            Collision = true;
    }
    }
};

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static uint64_t lastTime = 0;
static ObjectMovement* box = NULL;  // Add this

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

    box = new ObjectMovement();  // Create the box

    // Randomiser
    srand(time(NULL));

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
    box->update(deltaTime);

    if (box->Collision == true)
    {
        box->randomizeColor();
        box->Collision = false;
    }

    
    // Clear and draw
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, box->r, box->g, box->b, SDL_ALPHA_OPAQUE);
    SDL_RenderRect(renderer, &box->myRect);
    
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}
