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

    ObjectMovement obj(&x, &y, 10.0f, 10.0f, 0);
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
    
    ObjectMovement obj(&x, &y, 10.0f, 10.0f, 10);
    obj.pos.vel_X = 50.0f;  // Set known velocity
    obj.pos.vel_Y = 0.0f;
    
    obj.updatePos(1.0f);  // Simulate 1 full second
    
    
    EXPECT_FLOAT_EQ(*obj.pos.x_pos, 150.0f);
    EXPECT_FLOAT_EQ(*obj.pos.y_pos, 100.981f); // This is due to gravity 0.0981m/s^2
}


/*------------------------------------------------------------------------------
*   Border Collision Tests
------------------------------------------------------------------------------*/

TEST(CollisionTest, CheckLeftBorderCollision)
{
    float x = 100.0f;
    float y = 100.0f;
    ObjectMovement obj(&x, &y, 100.0f, 100.0f, 20);
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
    ObjectMovement obj(&x, &y, 100.0f, 100.0f, 20);
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

TEST(CollisionTest, CheckTopBorderCollision)
{
    float x = 100.0f;
    float y = 100.0f;
    ObjectMovement obj(&x, &y, 100.0f, 100.0f, 20);
    obj.pos.vel_X = 0.0f;
    obj.pos.vel_Y = -50.0f;
    *obj.pos.x_pos = 200.0f;
    *obj.pos.y_pos = 150.0f;

    obj.updateContactPoints();
    collisionSystem->CheckBorderCollision(&obj);
    obj.updatePos(1.0f);       

    obj.updateContactPoints();
    collisionSystem->CheckBorderCollision(&obj);
    obj.updatePos(1.0f);      

    obj.updateContactPoints();
    collisionSystem->CheckBorderCollision(&obj);
    obj.updatePos(1.0f);        
   
    obj.updateContactPoints();
    EXPECT_FLOAT_EQ(obj.contact.topContact, 38.8228f); // This is due to gravity 0.0981m/s^2

}

/*------------------------------------------------------------------------------
*   Object Collision Tests
------------------------------------------------------------------------------*/


TEST(CollisionTest, CheckX_ObjectCollision)
{
    // Place two objects overlapping on the X axis only
    // obj1 moving right, obj2 stationary
    // obj1 centre at (100, 300), half-extent 20  -> right contact = 120
    // obj2 centre at (115, 300), half-extent 20  -> left  contact = 95
    // X overlap is small (25), Y overlap is large (40), so X axis should resolve

    float x1 = 100.0f, y1 = 300.0f;
    float x2 = 115.0f, y2 = 300.0f;

    ObjectMovement obj1(&x1, &y1, 20.0f, 20.0f, 10.0f);
    ObjectMovement obj2(&x2, &y2, 20.0f, 20.0f, 10.0f);

    obj1.pos.vel_X = 50.0f;
    obj1.pos.vel_Y = 0.0f;
    obj2.pos.vel_X = 0.0f;
    obj2.pos.vel_Y = 0.0f;

    obj1.updateContactPoints();
    obj2.updateContactPoints();

    Collisions collisions;
    collisions.CheckObjectCollision(&obj1, &obj2);

    // Equal masses: velocities should exchange on X axis
    // obj1 vel_X should become 0, obj2 vel_X should become 50
    EXPECT_FLOAT_EQ(obj1.pos.vel_X,  0.0f);
    EXPECT_FLOAT_EQ(obj2.pos.vel_X, 50.0f);

    // Y velocities should be completely untouched
    EXPECT_FLOAT_EQ(obj1.pos.vel_Y, 0.0f);
    EXPECT_FLOAT_EQ(obj2.pos.vel_Y, 0.0f);
}

TEST(CollisionTest, CheckY_ObjectCollision)
{
    // Place two objects overlapping on the Y axis only
    // obj1 moving downward, obj2 stationary
    // obj1 centre (300, 100), half-extent 20 -> bottom = 120
    // obj2 centre (300, 115), half-extent 20 -> top    = 95
    // Y overlap is small (25), X overlap is large (40), so Y axis should resolve

    float x1 = 300.0f, y1 = 100.0f;
    float x2 = 300.0f, y2 = 115.0f;

    ObjectMovement obj1(&x1, &y1, 20.0f, 20.0f, 10.0f);
    ObjectMovement obj2(&x2, &y2, 20.0f, 20.0f, 10.0f);

    obj1.pos.vel_X = 0.0f;
    obj1.pos.vel_Y = 50.0f;
    obj2.pos.vel_X = 0.0f;
    obj2.pos.vel_Y = 0.0f;

    obj1.updateContactPoints();
    obj2.updateContactPoints();

    Collisions collisions;
    collisions.CheckObjectCollision(&obj1, &obj2);

    // Equal masses: velocities should exchange on Y axis
    EXPECT_FLOAT_EQ(obj1.pos.vel_Y,  0.0f);
    EXPECT_FLOAT_EQ(obj2.pos.vel_Y, 50.0f);

    // X velocities should be untouched (note: constructor sets vel_X = 200,
    // so check they match whatever was set above)
    EXPECT_FLOAT_EQ(obj1.pos.vel_X, 0.0f);
    EXPECT_FLOAT_EQ(obj2.pos.vel_X, 0.0f);
}

TEST(CollisionTest, NoCollisionWhenSeparate)
{
    // Objects clearly not overlapping - nothing should change
    float x1 = 100.0f, y1 = 100.0f;
    float x2 = 400.0f, y2 = 400.0f;

    ObjectMovement obj1(&x1, &y1, 20.0f, 20.0f, 10.0f);
    ObjectMovement obj2(&x2, &y2, 20.0f, 20.0f, 10.0f);

    obj1.pos.vel_X = 50.0f;
    obj1.pos.vel_Y = 0.0f;
    obj2.pos.vel_X = -50.0f;
    obj2.pos.vel_Y = 0.0f;

    obj1.updateContactPoints();
    obj2.updateContactPoints();

    Collisions collisions;
    collisions.CheckObjectCollision(&obj1, &obj2);

    // Velocities must be completely unchanged
    EXPECT_FLOAT_EQ(obj1.pos.vel_X,  50.0f);
    EXPECT_FLOAT_EQ(obj2.pos.vel_X, -50.0f);
}

TEST(CollisionTest, UnequalMassCollision)
{
    // Verify conservation of momentum with unequal masses on X axis
    // obj1 (mass 10) moving right at 60, obj2 (mass 30) stationary
    // Expected post-collision velocities from elastic formula:
    //   v1' = ((10-30)*60 + 2*30*0) / (10+30) = (-1200)/40 = -30
    //   v2' = ((30-10)*0  + 2*10*60)/ (10+30) = 1200/40    = +30

    float x1 = 100.0f, y1 = 300.0f;
    float x2 = 115.0f, y2 = 300.0f;

    ObjectMovement obj1(&x1, &y1, 20.0f, 20.0f, 10.0f);
    ObjectMovement obj2(&x2, &y2, 20.0f, 20.0f, 30.0f);

    obj1.pos.vel_X = 60.0f;
    obj1.pos.vel_Y = 0.0f;
    obj2.pos.vel_X = 0.0f;
    obj2.pos.vel_Y = 0.0f;

    obj1.updateContactPoints();
    obj2.updateContactPoints();

    Collisions collisions;
    collisions.CheckObjectCollision(&obj1, &obj2);

    EXPECT_FLOAT_EQ(obj1.pos.vel_X, -30.0f);
    EXPECT_FLOAT_EQ(obj2.pos.vel_X,  30.0f);
}