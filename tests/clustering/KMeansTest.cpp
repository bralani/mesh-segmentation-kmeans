#include <gtest/gtest.h>
#include "clustering/KMeans.hpp"

// Define test fixture for KMeans
class KMeansTest : public ::testing::Test
{
protected:
    using Point2D = Point<double, 2>;
    using Metric2D = EuclideanMetric<double, 2>;
    std::vector<Point2D> points;
    Metric2D *metric;

    void SetUp() override
    {
        // Create some sample points
        points.push_back(Point2D({1.0, 2.0}, -1));
        points.push_back(Point2D({2.0, 3.0}, -1));
        points.push_back(Point2D({3.0, 4.0}, -1));
        points.push_back(Point2D({5.0, 8.0}, -1));

        // Initialize metric
        metric = new Metric2D(points, 0.01);
    }

    void TearDown() override
    {
        delete metric;
    }
};

// Test Constructor
TEST_F(KMeansTest, ConstructorInitializesProperly)
{
    KMeans<double, 2, Metric2D> kmeans(2, 0.001, metric, 0, 0);

    EXPECT_EQ(kmeans.getCentroids().size(), 2);
}

// Test GetPoints
TEST_F(KMeansTest, GetPointsReturnsCorrectValues)
{
    KMeans<double, 2, Metric2D> kmeans(2, 0.001, metric, 0, 0);
    auto &retrievedPoints = kmeans.getPoints();

    ASSERT_EQ(retrievedPoints.size(), points.size());
    EXPECT_EQ(retrievedPoints[0].coordinates[0], 1.0);
}

// Test Reset Centroids
TEST_F(KMeansTest, ResetCentroidsClearsData)
{
    KMeans<double, 2, Metric2D> kmeans(2, 0.001, metric, 0, 0);
    kmeans.resetCentroids();

    EXPECT_TRUE(kmeans.getCentroids().empty());
}

// Test Fitting
TEST_F(KMeansTest, FitRunsWithoutError)
{
    KMeans<double, 2, Metric2D> kmeans(2, 0.001, metric, 0, 0);
    EXPECT_NO_THROW(kmeans.fit());
}

// Test Changing Number of Clusters keeps still max number of dimension allowed
TEST_F(KMeansTest, SetNumClustersWorks)
{
    KMeans<double, 2, Metric2D> kmeans(2, 0.001, metric, 0, 0);
    kmeans.setNumClusters(3);

    EXPECT_EQ(kmeans.getCentroids().size(), 2);
}
