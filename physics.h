/*============================================================================ 
*    FILE: physics.h
*
*    DESCRIPTION:
*
*    This is where the physics of the objects within the screen is computed.
*    It includes : movement, collisions, height width etc.
*
*=============================================================================*/

#pragma once

// Standard Libraries
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

// App Window Screen size
#define WINDOW_WIDTH    600
#define WINDOW_HEIGHT   600



/*=====================================================================
*   SECTION: Object Colouring and Creation   
*=====================================================================*/

typedef struct{
    float topContact;
    float bottomContact;
    float leftContact;
    float rightContact;
    float force_x;
    float force_y;
} ContactPoints;

typedef struct 
{
    float* x_pos;
    float* y_pos;
    float vel_X;
    float vel_Y;
} PositionVar;

/* Circle Object Class */
class Circles
{
    public:
    // Circle Position
    float circle_x;
    float circle_y;
    float circle_radius;

    // Contact Points
    float circle_left;
    float circle_right;
    float circle_top;
    float circle_bottom;

    // Physics Var
    float c_mass;


    // Circle Initialise
    Circles() 
    {
        circle_x        = 300.0f;
        circle_y        = 300.0f;
        circle_radius   = 40.0f;

        c_mass = 30.0f;
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

    float rect_mass;

    Rectangles()
    {
        rect_x = 225;
        rect_y = 35; 
        rect_w = 25;
        rect_h = 25;

        rect_mass = 10;
    }
};

// Define Shape Classes
static Rectangles* box = NULL;
static Circles* ball = NULL;

// Colour variables
static uint8_t r = 255;
static uint8_t g = 255;
static uint8_t b = 255;

// Change Object Colour
void randomiseColour() 
{
    r = rand() % 256;  // Random value 0-255
    g = rand() % 256;
    b = rand() % 256;
}

/*=====================================================================
*   SECTION: Object Movement and Collision Handling
*=====================================================================*/

/*=====================================================================
*   Class: ObjectMovement 
* 
*   Description: Handles the position and movements of objects created
*   in the SDL Rendering.
*=====================================================================*/
class ObjectMovement
{    
    public:
    PositionVar pos;
    ContactPoints contact;
    
    float gravity;

    float width;   
    float height;  
    float mass;
    float force_x;
    float force_y;

    ObjectMovement(float* x, float* y, float w, float h, float m) { 
        pos.x_pos   = x;  
        pos.y_pos   = y;
        width       = w;
        height      = h;
        mass        = m;

        pos.vel_X   = 200.0f;
        pos.vel_Y   = 0.0f;
        gravity     = 0.0981f;

        contact.force_x     = mass * pos.vel_X;
        contact.force_y     = mass * pos.vel_Y;
    }

    void updateContactPoints() 
    {
        contact.topContact      = *pos.y_pos - height;  
        contact.bottomContact   = *pos.y_pos + height;
        contact.leftContact     = *pos.x_pos - width;
        contact.rightContact    = *pos.x_pos + width;
    }

    void updatePos(float deltaTime) 
    {
        // Position update over time
        *pos.x_pos  += pos.vel_X * deltaTime;
        
        pos.vel_Y   += gravity * mass * deltaTime;   
        *pos.y_pos  += pos.vel_Y;
    }
};


/*  =======================================================================
    Class: Collision

    Description: 
  
    Handles the collision between objects after checking their positions
    and updating their direction and position accordingly
    ======================================================================== */

class Collisions
{
    

    public:
    void CheckObjectCollision(ObjectMovement* obj1, ObjectMovement* obj2)
    {
        // Check if collision exists
        if ((obj1->contact.rightContact  >= obj2->contact.leftContact)   && 
            (obj1->contact.leftContact   <= obj2->contact.rightContact)  &&
            (obj1->contact.bottomContact >= obj2->contact.topContact)    &&
            (obj1->contact.topContact    <= obj2->contact.bottomContact))
        {
            // Physics parameters for each object
            float v1x = obj1->pos.vel_X;
            float v2x = obj2->pos.vel_X;

            float v1y = obj1->pos.vel_Y;
            float v2y = obj2->pos.vel_Y;

            float m1  = obj1->mass;
            float m2  = obj2->mass;            

            // Calculating the overlap when the two objects collide
            float overlapLeft   = obj1->contact.leftContact  - obj2->contact.rightContact;
            float overlapRight  = obj1->contact.rightContact - obj2->contact.leftContact;
            float overlapTop    = obj1->contact.topContact   - obj2->contact.bottomContact;
            float overlapBottom = obj1->contact.bottomContact- obj2->contact.topContact;
            
            // Determine which axis has minimum overlap
            float absOverlapX = (overlapLeft < 0) ? -overlapLeft : overlapRight;
            float absOverlapY = (overlapTop < 0) ? -overlapTop : overlapBottom;

            // Resolve on the axis with least penetration using Conservation of Momentum and Kinetic Energy
            if (absOverlapX < absOverlapY) 
            {
                // X-axis collision
                obj1->pos.vel_X = ((m1 - m2) * v1x + 2 * m2 * v2x) / (m1 + m2);
                obj2->pos.vel_X = ((m2 - m1) * v2x + 2 * m1 * v1x) / (m1 + m2);
            } 
            else 
            {
                // Y-axis collision
                obj1->pos.vel_Y = ((m1 - m2) * v1y + 2 * m2 * v2y) / (m1 + m2);
                obj2->pos.vel_Y = ((m2 - m1) * v2y + 2 * m1 * v1y) / (m1 + m2);
            }
        }
    }

    void CheckBorderCollision(ObjectMovement* obj)
    {
        // Collision with left wall
        if (obj->contact.leftContact < 0.0) 
        {
            *(obj->pos.x_pos)   = 0 + obj->width;
            obj->pos.vel_X      = -obj->pos.vel_X;
        }
        
        // Collision with right wall
        if (obj->contact.rightContact > WINDOW_WIDTH) 
        {
            *(obj->pos.x_pos)   = WINDOW_WIDTH - obj->width;
            obj->pos.vel_X      = -obj->pos.vel_X;
        }
        
        // Top
        if (obj->contact.topContact < 0) 
        {
            *(obj->pos.y_pos)   = obj->height;
            obj->pos.vel_Y      = -obj->pos.vel_Y * 0.8f;
        }
        
        // Bottom
        if (obj->contact.bottomContact > WINDOW_HEIGHT) 
        {
            *(obj->pos.y_pos)   = WINDOW_HEIGHT - obj->height;
            obj->pos.vel_Y      = -obj->pos.vel_Y * 0.8f;
        }
    }
};