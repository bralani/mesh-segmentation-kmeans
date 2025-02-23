#include <gtest/gtest.h>
#include "clustering/CentroidInitializationMethods/KernelFunction.hpp"

class KernelTest : public ::testing::Test
{
protected:
    Eigen::VectorXd zero;
    Eigen::VectorXd inside;
    Eigen::VectorXd outside;

    void SetUp() override
    {
        zero = Eigen::VectorXd::Zero(3);
        inside = Eigen::VectorXd::Constant(3, 0.5);
        outside = Eigen::VectorXd::Constant(3, 1.5);
    }
};

TEST_F(KernelTest, GaussianKernel)
{
    EXPECT_NEAR(Kernel::gaussian(zero), 1.0 / pow(2 * M_PI, 1.5), 1e-5);
    EXPECT_GT(Kernel::gaussian(inside), 0);
    EXPECT_GT(Kernel::gaussian(outside), 0);
}

TEST_F(KernelTest, EpanechnikovKernel)
{
    EXPECT_GT(Kernel::epanechnikov(inside), 0);
    EXPECT_EQ(Kernel::epanechnikov(outside), 0);
}

TEST_F(KernelTest, UniformKernel)
{
    EXPECT_EQ(Kernel::uniform(inside), 0.5);
    EXPECT_EQ(Kernel::uniform(outside), 0.0);
}

TEST_F(KernelTest, TriangularKernel)
{
    EXPECT_GT(Kernel::triangular(inside), 0);
    EXPECT_EQ(Kernel::triangular(outside), 0);
}

TEST_F(KernelTest, BiweightKernel)
{
    EXPECT_GT(Kernel::biweight(inside), 0);
    EXPECT_EQ(Kernel::biweight(outside), 0);
}

TEST_F(KernelTest, TriweightKernel)
{
    EXPECT_GT(Kernel::triweight(inside), 0);
    EXPECT_EQ(Kernel::triweight(outside), 0);
}

TEST_F(KernelTest, CosineKernel)
{
    EXPECT_GT(Kernel::cosine(inside), 0);
    EXPECT_EQ(Kernel::cosine(outside), 0);
}
