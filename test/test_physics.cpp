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

TEST(ContactPoints, CalculatesCorrectEdges) {
    float x = 100.0f, y = 100.0f;
    ObjectMovement obj(&x, &y, 10.0f, 10.0f);

    obj.updateContactPoints();

    EXPECT_FLOAT_EQ(obj.leftContact,    90.0f);
    EXPECT_FLOAT_EQ(obj.rightContact,  110.0f);
    EXPECT_FLOAT_EQ(obj.topContact,     90.0f);
    EXPECT_FLOAT_EQ(obj.bottomContact, 110.0f);
}