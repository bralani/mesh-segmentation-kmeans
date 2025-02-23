#include <gtest/gtest.h>
#include "geometry/kdtree/KDNode.hpp"

// Test fixture for KdNode
class KdNodeTest : public ::testing::Test
{
};

// Test default constructor
TEST_F(KdNodeTest, DefaultConstructor)
{
    KdNode<double, 2> node;

    EXPECT_EQ(node.cellMin[0], 0);
    EXPECT_EQ(node.cellMin[1], 0);
    EXPECT_EQ(node.cellMax[0], 0);
    EXPECT_EQ(node.cellMax[1], 0);
}

// Test constructor with bounding box
TEST_F(KdNodeTest, BoundingBoxConstructor)
{
    std::array<double, 2> min = {1.0, 2.0};
    std::array<double, 2> max = {3.0, 4.0};
    KdNode<double, 2> node(min, max);

    EXPECT_EQ(node.cellMin[0], 1.0);
    EXPECT_EQ(node.cellMin[1], 2.0);
    EXPECT_EQ(node.cellMax[0], 3.0);
    EXPECT_EQ(node.cellMax[1], 4.0);
}

// Test constructor with 3D bounding box
TEST_F(KdNodeTest, BoundingBoxConstructor3D)
{
    std::array<double, 3> min = {0.5, 1.5, 2.5};
    std::array<double, 3> max = {3.5, 4.5, 5.5};
    KdNode<double, 3> node(min, max);

    EXPECT_EQ(node.cellMin[0], 0.5);
    EXPECT_EQ(node.cellMin[1], 1.5);
    EXPECT_EQ(node.cellMin[2], 2.5);
    EXPECT_EQ(node.cellMax[0], 3.5);
    EXPECT_EQ(node.cellMax[1], 4.5);
    EXPECT_EQ(node.cellMax[2], 5.5);
}
