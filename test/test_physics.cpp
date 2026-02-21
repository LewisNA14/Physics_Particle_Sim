/*============================================================================ 
*    FILE: test_physics.cpp
*
*    DESCRIPTION:
*
*    This is just a current example to try and make a box appear using SDL
*
*=============================================================================*/

// Standard Libraries
#include <gtest/gtest.h>
#include "../physics.h"

ContactPoints contact;
PositionVar pos;


// Static Variable Declarations
static Collisions* collisionSystem = NULL;

/*--------------------------------------------------------------------------------
    ObjectMovement Tests
----------------------------------------------------------------------------------*/
    TEST(ObjMoveTest, ContactPoints)
{
    float x = 100.0f;
    float y = 100.0f;

    ObjectMovement obj(&x, &y, 10.0f, 10.0f);
    obj.updateContactPoints();

    EXPECT_FLOAT_EQ(obj.contact.leftContact,    90.0f);
    EXPECT_FLOAT_EQ(obj.contact.rightContact,   110.0f);
    EXPECT_FLOAT_EQ(obj.contact.topContact,     90.0f);
    EXPECT_FLOAT_EQ(obj.contact.bottomContact,  110.0f);
}

TEST(ObjMoveTest, PositionChanges)
{
    float x = 100.0f;
    float y = 100.0f;
    
    ObjectMovement obj(&x, &y, 10.0f, 10.0f);
    obj.pos.vel_X = 50.0f;  // Set known velocity
    obj.pos.vel_Y = 0.0f;
    
    obj.updatePos(1.0f);  // Simulate 1 full second
    
    
    EXPECT_FLOAT_EQ(*obj.pos.x_pos, 150.0f);
    EXPECT_FLOAT_EQ(*obj.pos.y_pos, 100.0981f); // This is due to gravity 0.0981m/s^2
}


/*------------------------------------------------------------------------------
*   Collision Tests
------------------------------------------------------------------------------*/

TEST(CollisionTest, CheckLeftBorderCollision)
{
    float x = 100.0f;
    float y = 100.0f;
    ObjectMovement obj(&x, &y, 100.0f, 100.0f);
    obj.pos.vel_X = -50.0f;
    obj.pos.vel_Y = 0.0f;
    *obj.pos.x_pos = 150.0f;
    *obj.pos.y_pos = 100.0f;

    obj.updateContactPoints();
    collisionSystem->CheckBorderCollision(&obj);
    obj.updatePos(1.0f);        // x = 150 + (-50*1) = 100, leftContact = 0, no collision yet

    obj.updateContactPoints();
    collisionSystem->CheckBorderCollision(&obj);
    obj.updatePos(1.0f);        // x = 100 + (-50*1) = 50, leftContact = -50, collision
                                // resets x to 100, vel_X flips to +50

    obj.updateContactPoints();
    collisionSystem->CheckBorderCollision(&obj);
    obj.updatePos(1.0f);        // x = 100 + (50*1) = 150, leftContact = 50
   

    obj.updateContactPoints();
    EXPECT_FLOAT_EQ(obj.contact.leftContact, 50.0f);
}

TEST(CollisionTest, CheckRightBorderCollision)
{
    float x = 100.0f;
    float y = 100.0f;
    ObjectMovement obj(&x, &y, 100.0f, 100.0f);
    obj.pos.vel_X = 50.0f;
    obj.pos.vel_Y = 0.0f;
    *obj.pos.x_pos = 450.0f;
    *obj.pos.y_pos = 100.0f;

    obj.updateContactPoints();
    collisionSystem->CheckBorderCollision(&obj);
    obj.updatePos(1.0f);        // x = 150 + (-50*1) = 100, leftContact = 0, no collision yet

    obj.updateContactPoints();
    collisionSystem->CheckBorderCollision(&obj);
    obj.updatePos(1.0f);        // x = 100 + (-50*1) = 50, leftContact = -50, collision
                                // resets x to 100, vel_X flips to +50

    obj.updateContactPoints();
    collisionSystem->CheckBorderCollision(&obj);
    obj.updatePos(1.0f);        // x = 100 + (50*1) = 150, leftContact = 50
   

    obj.updateContactPoints();
    EXPECT_FLOAT_EQ(obj.contact.rightContact, 550.0f);
}