#include <gtest/gtest.h>
#include "geometry/metrics/Metric.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/point/CentroidPoint.hpp"
#include "geometry/point/Point.hpp"

class MetricTest : public ::testing::Test
{
protected:
    using TestMetric = EuclideanMetric<double, 2>;
    std::vector<CentroidPoint<double, 2>> centroids;
    std::vector<Point<double, 2>> points;

    void SetUp() override
    {
        centroids.push_back(CentroidPoint<double, 2>(Point<double, 2>({1.0, 2.0}, -1)));
        centroids.push_back(CentroidPoint<double, 2>(Point<double, 2>({3.0, 4.0}, -1)));

        points.push_back(Point<double, 2>({2.0, 3.0}, -1));
        points.push_back(Point<double, 2>({4.0, 5.0}, -1));
    }
};

// Test default constructor
TEST_F(MetricTest, DefaultConstructor)
{
    TestMetric metric;
    EXPECT_TRUE(metric.getPoints().empty());
}

// Test constructor with centroids
TEST_F(MetricTest, ConstructorWithCentroids)
{
    TestMetric metric;
    metric.setCentroids(centroids);
    EXPECT_EQ(centroids.size(), 2);
}

// Test constructor with centroids and data points
TEST_F(MetricTest, ConstructorWithCentroidsAndData)
{
    TestMetric metric;
    metric.setCentroids(centroids);
    metric.setPoints(points);
    EXPECT_EQ(metric.getPoints().size(), 2);
}

// Test setPoints and getPoints
TEST_F(MetricTest, SetAndGetPoints)
{
    TestMetric metric;
    metric.setPoints(points);
    EXPECT_EQ(metric.getPoints().size(), 2);
    EXPECT_EQ(metric.getPoints()[0].coordinates[0], 2.0);
}

// Test setting centroids
TEST_F(MetricTest, SetCentroids)
{
    TestMetric metric;
    metric.setCentroids(centroids);
    EXPECT_EQ(centroids.size(), 2);
}

// Test resetCentroids
TEST_F(MetricTest, ResetCentroids)
{
    TestMetric metric;
    metric.setCentroids(centroids);
    metric.setPoints(points);
    metric.resetCentroids();

    for (const auto &p : metric.getPoints())
    {
        EXPECT_EQ(p.centroid, nullptr);
    }
}
