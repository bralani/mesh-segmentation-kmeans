#include <gtest/gtest.h>
#include "clustering/CentroidInitializationMethods/MostDistantCentroids.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"

TEST(MostDistantCentroidsTest, InitializeWithSinglePoint)
{
    std::vector<Point<double, 2>> data = {Point<double, 2>({0.0, 0.0}, -1)};
    MostDistanceClass<2> initializer(data, 1);
    std::vector<CentroidPoint<double, 2>> centroids;
    initializer.findCentroid(centroids);
    ASSERT_EQ(centroids.size(), 1);
    EXPECT_EQ(centroids[0].coordinates[0], data[0].coordinates[0]);
    EXPECT_EQ(centroids[0].coordinates[1], data[0].coordinates[1]);
}

TEST(MostDistantCentroidsTest, InitializeWithTwoPoints)
{
    std::vector<Point<double, 2>> data = {Point<double, 2>({0.0, 0.0}, -1), Point<double, 2>({1.0, 1.0}, -1)};
    MostDistanceClass<2> initializer(data, 2);
    std::vector<CentroidPoint<double, 2>> centroids;
    initializer.findCentroid(centroids);
    ASSERT_EQ(centroids.size(), 2);
    EXPECT_NE(centroids[0].coordinates, centroids[1].coordinates);
}

TEST(MostDistantCentroidsTest, InitializeWithMultiplePoints)
{
    std::vector<Point<double, 2>> data = {
        Point<double, 2>({0.0, 0.0}, -1),
        Point<double, 2>({1.0, 1.0}, -1),
        Point<double, 2>({2.0, 2.0}, -1),
        Point<double, 2>({-1.0, -1.0}, -1)};
    MostDistanceClass<2> initializer(data, 3);
    std::vector<CentroidPoint<double, 2>> centroids;
    initializer.findCentroid(centroids);
    ASSERT_EQ(centroids.size(), 3);
    // Check that centroids are distinct and far apart
    for (size_t i = 0; i < centroids.size(); ++i)
    {
        for (size_t j = i + 1; j < centroids.size(); ++j)
        {
            double distance = EuclideanMetric<double, 2>::distanceTo(centroids[i], centroids[j]);
            EXPECT_GT(distance, 0.5);
        }
    }
}

TEST(MostDistantCentroidsTest, InitializeWithDuplicatePoints)
{
    std::vector<Point<double, 2>> data = {
        Point<double, 2>({0.0, 0.0}, -1),
        Point<double, 2>({0.0, 0.0}, -1),
        Point<double, 2>({1.0, 1.0}, -1),
        Point<double, 2>({1.0, 1.0}, -1)};
    MostDistanceClass<2> initializer(data, 2);
    std::vector<CentroidPoint<double, 2>> centroids;
    initializer.findCentroid(centroids);
    ASSERT_EQ(centroids.size(), 2);
    EXPECT_NE(centroids[0].coordinates, centroids[1].coordinates);
}
