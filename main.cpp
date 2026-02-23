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
#include <vector>
#include <memory>

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
*   SECTION: Object pool structs
*
*   Each pool entry owns the shape data AND its ObjectMovement so that
*   pointers stored inside ObjectMovement remain stable while the vector
*   grows (we use unique_ptr to keep heap addresses fixed).
* =======================================================================*/

struct BallEntry
{
    std::unique_ptr<Circles>        shape;
    std::unique_ptr<ObjectMovement> mover;
};

struct BoxEntry
{
    std::unique_ptr<Rectangles>     shape;
    std::unique_ptr<ObjectMovement> mover;
};


/*=======================================================================
*  SECTION: Global Variables
* =======================================================================*/
    
static SDL_Window*   window   = NULL;
static SDL_Renderer* renderer = NULL;
static Collisions*   collisionSystem = NULL;
static uint64_t      lastTime = 0;

// Dynamic object pools
static std::vector<BallEntry> balls;
static std::vector<BoxEntry>  boxes;

// Random float in [lo, hi]
static float randRange(float lo, float hi)
{
    return lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
}

/*=======================================================================
*   SECTION: Spawn / Remove helpers
* =======================================================================*/

static void spawnBall()
{
    BallEntry entry;
    entry.shape = std::make_unique<Circles>(
        randRange(60.0f, WINDOW_WIDTH  - 60.0f),   // x
        randRange(60.0f, WINDOW_HEIGHT - 60.0f),   // y
        40.0f,                                      // radius
        30.0f                                       // mass
    );

    entry.mover = std::make_unique<ObjectMovement>(
        &entry.shape->circle_x,
        &entry.shape->circle_y,
        entry.shape->circle_radius,
        entry.shape->circle_radius,
        entry.shape->c_mass
    );

    // Random initial velocity so objects don't all stack
    entry.mover->pos.vel_X = randRange(-200.0f, 200.0f);
    entry.mover->pos.vel_Y = randRange(-50.0f,  50.0f);

    balls.push_back(std::move(entry));
    SDL_Log("Ball spawned  — total balls: %zu", balls.size());
}

static void removeBall()
{
    if (!balls.empty())
    {
        balls.pop_back();
        SDL_Log("Ball removed  — total balls: %zu", balls.size());
    }
}

static void spawnBox()
{
    BoxEntry entry;
    entry.shape = std::make_unique<Rectangles>(
        randRange(30.0f, WINDOW_WIDTH  - 30.0f),   // x
        randRange(30.0f, WINDOW_HEIGHT - 30.0f),   // y
        25.0f,                                      // half-width
        25.0f,                                      // half-height
        10.0f                                       // mass
    );

    entry.mover = std::make_unique<ObjectMovement>(
        &entry.shape->rect_x,
        &entry.shape->rect_y,
        entry.shape->rect_w,
        entry.shape->rect_h,
        entry.shape->rect_mass
    );

    entry.mover->pos.vel_X = randRange(-200.0f, 200.0f);
    entry.mover->pos.vel_Y = randRange(-50.0f,  50.0f);

    boxes.push_back(std::move(entry));
    SDL_Log("Box spawned   — total boxes: %zu", boxes.size());
}

static void removeBox()
{
    if (!boxes.empty())
    {
        boxes.pop_back();
        SDL_Log("Box removed   — total boxes: %zu", boxes.size());
    }
}

/*=======================================================================
*   SECTION: SDL Callbacks
* =======================================================================*/

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
     srand((unsigned int)time(NULL));

    SDL_SetAppMetadata("Physics Sim", "1.0", "com.example.physicssim");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialise SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Physics Sim", WINDOW_WIDTH, WINDOW_HEIGHT,
                                     SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    collisionSystem = new Collisions();
    lastTime = SDL_GetTicks();

    // Start with one of each so the screen isn't empty
    spawnBall();
    spawnBox();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        const bool shift = (event->key.mod & SDL_KMOD_SHIFT) != 0;

        switch (event->key.key)
        {
            case SDLK_B:
                shift ? removeBall() : spawnBall();
                break;

            case SDLK_X:
                shift ? removeBox() : spawnBox();
                break;

            default:
                break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    // Time keeping 
    uint64_t now       = SDL_GetTicks();
    float    deltaTime = (now - lastTime) / 1000.0f;
    lastTime = now;

    // Gather all movers into a flat list for collision checks 
    std::vector<ObjectMovement*> allMovers;
    allMovers.reserve(balls.size() + boxes.size());

    for (auto& b : balls)  allMovers.push_back(b.mover.get());
    for (auto& b : boxes)  allMovers.push_back(b.mover.get());

    // Physics update 
    for (auto* m : allMovers)
    {
        m->updatePos(deltaTime);
        m->updateContactPoints();
        collisionSystem->CheckBorderCollision(m);
    }

    // object–object collision check 
    for (size_t i = 0; i < allMovers.size(); ++i)
        for (size_t j = i + 1; j < allMovers.size(); ++j)
            collisionSystem->CheckObjectCollision(allMovers[i], allMovers[j]);

    // Render 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Draw balls
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, SDL_ALPHA_OPAQUE);
    for (auto& b : balls)
        DrawCircle(renderer,
                   (int)b.shape->circle_x,
                   (int)b.shape->circle_y,
                   (int)b.shape->circle_radius);

    // Draw boxes
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    for (auto& b : boxes)
        DrawRectangle(renderer,
                      (int)b.shape->rect_x,
                      (int)b.shape->rect_y,
                      (int)b.shape->rect_w,
                      (int)b.shape->rect_h);

    char hud[128];

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugText(renderer, 10.0f, 10.0f, "B = Spawn Ball    Shift+B = Remove Ball");
    SDL_RenderDebugText(renderer, 10.0f, 25.0f, "X = Spawn Box     Shift+X = Remove Box");

    SDL_snprintf(hud, sizeof(hud), "Balls: %zu   Boxes: %zu", balls.size(), boxes.size());
    SDL_RenderDebugText(renderer, 10.0f, 45.0f, hud);

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // unique_ptrs clean themselves up; just clear the vectors
    balls.clear();
    boxes.clear();

    delete collisionSystem;
    collisionSystem = NULL;
}
