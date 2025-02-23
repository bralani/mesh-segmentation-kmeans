
#include <gtest/gtest.h>
#include "clustering/CentroidInitializationMethods/KDECentroidMatrix.hpp"

class KDE3DTest : public ::testing::Test
{
protected:
    std::vector<Point<double, 3>> sampleData;
    KDE3D *kde3d;

    void SetUp() override
    {
        // Create sample points for testing
        sampleData = {
            {{0.0, 0.0, 0.0}}, {{1.0, 1.0, 1.0}}, {{2.0, 2.0, 2.0}}, {{3.0, 3.0, 3.0}}, {{4.0, 4.0, 4.0}}, {{5.0, 5.0, 5.0}}};
        kde3d = new KDE3D(sampleData, 3); // Initialize KDE3D with k = 3
    }

    void TearDown() override
    {
        delete kde3d;
    }
};

// Test grid generation
TEST_F(KDE3DTest, GenerateGridTest)
{
    Grid3D grid = kde3d->generateGrid();
    ASSERT_FALSE(grid.empty());
    EXPECT_GT(grid.size(), 0);
    EXPECT_GT(grid[0].size(), 0);
    EXPECT_GT(grid[0][0].size(), 0);
}

// Test KDE density computation
TEST_F(KDE3DTest, KDEValueTest)
{
    Point<double, 3> testPoint = {{2.0, 2.0, 2.0}};
    double density = kde3d->kdeValue(testPoint);
    EXPECT_GT(density, 0.0);
}

// Test finding local maxima
TEST_F(KDE3DTest, FindLocalMaximaTest)
{
    Grid3D grid = kde3d->generateGrid();
    std::vector<CentroidPoint<double, 3>> centroids;
    kde3d->findLocalMaxima(grid, centroids);
    EXPECT_FALSE(centroids.empty());
}

// Test centroid selection
TEST_F(KDE3DTest, FindCentroidTest)
{
    std::vector<CentroidPoint<double, 3>> centroids;
    kde3d->findCentroid(centroids);
    EXPECT_EQ(centroids.size(), 3);
}
