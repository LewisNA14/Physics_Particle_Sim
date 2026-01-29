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

// Circle Struct
typedef struct {
    float x;
    float y;
    float r;
} Circle;

// Rectangle Movement and Collisions
class ObjectMovement
{
    public:
    SDL_FRect myRect;
    
    // Object Movement
    float vel_X;  
    float vel_Y;
    float gravity;

    // Reactive Movement
    
    // Randomiser
    
    
    uint8_t r, g, b;  // RGB color values

    ObjectMovement() {
        srand(time(NULL));

        myRect = {100, 100, 50, 50};
        myRect.x = 200;
        myRect.y = 10;

        Circle c = {200.0f, 150.0f, 40.0f}; 

        vel_X = (rand() % 1001)-500;           // 100 pixels per second
        vel_Y = 0.0f;
        // vel_Z = 0.0f;

        gravity = 98.1f;       // 98.1 Pixels per second to emulate gravity
        
        printf("Speed is: %f \n", vel_X);

        randomiseColour();
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

    // Change Object Colour
    void randomiseColour() 
    {
        r = rand() % 256;  // Random value 0-255
        g = rand() % 256;
        b = rand() % 256;
    }
    
    // Updates the position
    void update(float deltaTime) {
        
        // Move
        myRect.x += vel_X * deltaTime;

        vel_Y += gravity * deltaTime;
        myRect.y += vel_Y * deltaTime;
        
        // Bounce off edges - LEFT and RIGHT
        if (myRect.x < 0) 
        {
            myRect.x = 0;                       // Push back inside
            vel_X = -vel_X;
            randomiseColour();
        }
        if (myRect.x + myRect.w > WINDOW_WIDTH) 
        {
            myRect.x = WINDOW_WIDTH - myRect.w;  // Push back inside
            vel_X = -vel_X * 1.2;
            randomiseColour();
        }
        
        // Bounce off edges - TOP and BOTTOM

        // Top
        if (myRect.y < 0) 
        {
            myRect.y = 0;  // Push back inside

            randomiseColour();
        }

        // Bottom
        if (myRect.y + myRect.h > WINDOW_HEIGHT) 
        {
            myRect.y = WINDOW_HEIGHT - myRect.h;  // Push back inside
            vel_Y = -vel_Y * 1.0f;
            randomiseColour();
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

    // Clear and draw
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, box->r, box->g, box->b, SDL_ALPHA_OPAQUE);
    SDL_RenderRect(renderer, &box->myRect);

    // box->DrawCircle(renderer, &Circle c);
    
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}
