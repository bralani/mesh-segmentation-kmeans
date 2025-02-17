#include <gtest/gtest.h>
#include "geometry/metrics/EuclideanMetric.hpp"

// Define a simple test fixture
class EuclideanMetricTest : public ::testing::Test
{
protected:
    using Point2D = Point<double, 2>;
    std::vector<Point2D> samplePoints;
    EuclideanMetric<double, 2> *metric;

    void SetUp() override
    {
        samplePoints = {Point2D({0.0, 0.0}), Point2D({3.0, 4.0}), Point2D({6.0, 8.0})};
        metric = new EuclideanMetric<double, 2>(samplePoints, 0.1);
    }

    void TearDown() override
    {
        delete metric;
    }
};

// Test constructor
TEST_F(EuclideanMetricTest, ConstructorInitializesDataCorrectly)
{
    EXPECT_EQ(metric->getPoints().size(), samplePoints.size());
    for (size_t i = 0; i < samplePoints.size(); ++i)
    {
        EXPECT_EQ(metric->getPoints()[i].coordinates, samplePoints[i].coordinates);
    }
}

// Test Euclidean distance calculation
TEST_F(EuclideanMetricTest, DistanceToCalculatesCorrectly)
{
    Point2D a({0.0, 0.0});
    Point2D b({3.0, 4.0});
    EXPECT_DOUBLE_EQ(metric->distanceTo(a, b), 5.0);
}

TEST_F(EuclideanMetricTest, DistanceToThrowsForDifferentDimensions)
{
    Point<double, 3> p1({0.0, 0.0, 0.0});
    Point<double, 3> p2({1.0, 1.0}); // Incorrect dimension
    EXPECT_THROW(metric->distanceTo(p1, p2), std::invalid_argument);
}

// Test threshold value
TEST_F(EuclideanMetricTest, ThresholdInitialization)
{
    EXPECT_DOUBLE_EQ(metric->treshold, 0.1);
}
