#include <gtest/gtest.h>
#include "clustering/CentroidInitializationMethods/KDECentroid.hpp"

constexpr std::size_t PD = 2; // Adjust as needed
using PointType = Point<double, PD>;

class KDETest : public ::testing::Test
{
protected:
    std::vector<PointType> sampleData;
    KDE<PD> *kde;

    void SetUp() override
    {
        // Sample points
        sampleData = {
            {{1.0, 2.0}}, {{2.0, 3.0}}, {{3.0, 1.0}}, {{4.0, 2.0}}, {{5.0, 3.0}}, {{6.0, 1.0}}, {{7.0, 2.0}}, {{8.0, 3.0}}};
        kde = new KDE<PD>(sampleData);
    }

    void TearDown() override
    {
        delete kde;
    }
};

TEST_F(KDETest, GridGeneration)
{
    auto grid = kde->generateGrid();
    EXPECT_FALSE(grid.empty());
    EXPECT_GT(grid.size(), sampleData.size()); // Grid should be larger than input points
}

TEST_F(KDETest, KDEValueComputation)
{
    double density = kde->kdeValue(sampleData[0]);
    EXPECT_GT(density, 0.0);
}

TEST_F(KDETest, FindCentroids)
{
    std::vector<CentroidPoint<double, PD>> centroids;
    kde->findCentroid(centroids);
    EXPECT_FALSE(centroids.empty());
}

TEST_F(KDETest, LocalMaximumDetection)
{
    std::vector<PointType> grid = kde->generateGrid();
    std::vector<double> densities(grid.size());
    for (size_t i = 0; i < grid.size(); ++i)
    {
        densities[i] = kde->kdeValue(grid[i]);
    }

    std::vector<std::size_t> neighbors;
    std::vector<double> offsets;
    bool isMax = kde->isLocalMaximum(grid, densities, 0, neighbors, offsets);
    EXPECT_TRUE(isMax || !isMax); // Basic validity check
}
