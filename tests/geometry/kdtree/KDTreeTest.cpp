#include <gtest/gtest.h>
#include "geometry/kdtree/KDTree.hpp"
#include "geometry/kdtree/KDNode.hpp"
#include <vector>

// Test fixture for KdTree
class KdTreeTest : public ::testing::Test
{
};

// Test KD-tree construction with an empty vector
TEST_F(KdTreeTest, EmptyTree)
{
    std::vector<Point<double, 2>> points;
    KdTree<double, 2> tree(points);
    EXPECT_EQ(tree.getRoot(), nullptr);
}

// Test KD-tree construction with a single point
TEST_F(KdTreeTest, SinglePointTree)
{
    std::vector<Point<double, 2>> points = {Point<double, 2>({1.0, 2.0}, -1)};
    KdTree<double, 2> tree(points);
    auto root = std::move(tree.getRoot());
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->count, 1);
    EXPECT_EQ(root->wgtCent[0], 1.0);
    EXPECT_EQ(root->wgtCent[1], 2.0);
}

// Test KD-tree construction with multiple points
TEST_F(KdTreeTest, MultiPointTree)
{
    std::vector<Point<double, 2>> points = {
        Point<double, 2>({3.0, 1.0}, -1),
        Point<double, 2>({2.0, 4.0}, -1),
        Point<double, 2>({5.0, 2.0}, -1),
        Point<double, 2>({1.0, 3.0}, -1)};

    KdTree<double, 2> tree(points);
    auto root = std::move(tree.getRoot());
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->count, 4);
    EXPECT_GE(root->cellMax[0], root->cellMin[0]);
    EXPECT_GE(root->cellMax[1], root->cellMin[1]);
}

// Test KD-tree clearing
TEST_F(KdTreeTest, ClearTree)
{
    std::vector<Point<double, 2>> points = {
        Point<double, 2>({1.0, 2.0}, -1),
        Point<double, 2>({3.0, 4.0}, -1)};

    KdTree<double, 2> tree(points);
    tree.~KdTree();
    EXPECT_EQ(tree.getRoot(), nullptr);
}
