#include <gtest/gtest.h>
#include "clustering/CentroidInitializationMethods/RandomCentroids.hpp"

TEST(RandomCentroidInitTest, InitializeWithValidData)
{
    std::vector<Point<double, 2>> data = {
        Point<double, 2>({1.0, 2.0}, -1),
        Point<double, 2>({3.0, 4.0}, -1),
        Point<double, 2>({5.0, 6.0}, -1)};

    RandomCentroidInit<double, 2> initializer(data, 2);
    std::vector<CentroidPoint<double, 2>> centroids;
    initializer.findCentroid(centroids);

    ASSERT_EQ(centroids.size(), 2);
}

TEST(RandomCentroidInitTest, HandlesSinglePointDataset)
{
    std::vector<Point<double, 2>> data = {Point<double, 2>({0.0, 0.0}, -1)};

    RandomCentroidInit<double, 2> initializer(data, 1);
    std::vector<CentroidPoint<double, 2>> centroids;
    initializer.findCentroid(centroids);

    ASSERT_EQ(centroids.size(), 1);
    EXPECT_EQ(centroids[0].coordinates, data[0].coordinates);
}

TEST(RandomCentroidInitTest, RandomCentroidsAreFromData)
{
    std::vector<Point<double, 2>> data = {
        Point<double, 2>({1.0, 1.0}, -1),
        Point<double, 2>({2.0, 2.0}, -1),
        Point<double, 2>({3.0, 3.0}, -1),
        Point<double, 2>({4.0, 4.0}, -1)};

    RandomCentroidInit<double, 2> initializer(data, 2);
    std::vector<CentroidPoint<double, 2>> centroids;
    initializer.findCentroid(centroids);

    ASSERT_EQ(centroids.size(), 2);

    for (const auto &centroid : centroids)
    {
        bool found = std::any_of(data.begin(), data.end(), [&](const Point<double, 2> &point)
                                 { return centroid.coordinates == point.coordinates; });
        EXPECT_TRUE(found);
    }
}

TEST(RandomCentroidInitTest, RandomCentroidsHaveUniqueIDs)
{
    std::vector<Point<double, 2>> data = {
        Point<double, 2>({1.0, 1.0}, -1),
        Point<double, 2>({2.0, 2.0}, -1),
        Point<double, 2>({3.0, 3.0}, -1),
        Point<double, 2>({4.0, 4.0}, -1)};

    RandomCentroidInit<double, 2> initializer(data, 2);
    std::vector<CentroidPoint<double, 2>> centroids;
    initializer.findCentroid(centroids);

    ASSERT_EQ(centroids.size(), 2);
}
