#include <gtest/gtest.h>
#include "clustering/CentroidInitializationMethods/KDEBase.hpp"

// Fixture class for KDEBase tests
template <std::size_t PD>
class KDEBaseTest : public ::testing::Test
{
protected:
    std::vector<Point<double, PD>> sampleData;
    KDEBase<PD> kde;

    KDEBaseTest() : kde()
    {
        sampleData = {
            Point<double, PD>({1.0, 2.0}, -1),
            Point<double, PD>({2.0, 3.0}, -1),
            Point<double, PD>({3.0, 4.0}, -1),
            Point<double, PD>({4.0, 5.0}, -1)};
    }
};

using KDEBase2DTest = KDEBaseTest<2>;

TEST_F(KDEBase2DTest, ComputeMeanAndStdDev)
{
    auto [mean, stdDev] = kde.computeMeanAndStdDev(0, sampleData);
    EXPECT_NEAR(mean, 2.5, 1e-3);
    EXPECT_NEAR(stdDev, 1.118, 1e-3);
}

TEST_F(KDEBase2DTest, BandwidthRuleOfThumb)
{
    Eigen::MatrixXd bandwidth = kde.bandwidth_RuleOfThumb(sampleData);
    EXPECT_EQ(bandwidth.rows(), 2);
    EXPECT_EQ(bandwidth.cols(), 2);
    EXPECT_GT(bandwidth.determinant(), 0.0);
}

TEST_F(KDEBase2DTest, PointToVector)
{
    Eigen::VectorXd vec = kde.pointToVector(sampleData[0]);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_DOUBLE_EQ(vec[0], 1.0);
    EXPECT_DOUBLE_EQ(vec[1], 2.0);
}