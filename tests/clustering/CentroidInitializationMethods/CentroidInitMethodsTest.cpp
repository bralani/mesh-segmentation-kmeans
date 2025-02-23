#include <gtest/gtest.h>
#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"

template <typename PT, std::size_t PD>
class ConcreteCentroidInit : public CentroidInitMethod<PT, PD>
{
public:
    using CentroidInitMethod<PT, PD>::CentroidInitMethod; // Inherit constructor

    void findCentroid(std::vector<CentroidPoint<PT, PD>> &centroids) override
    {
        // Example: Just copying the first K points (you should implement proper logic)
        centroids.clear();
        for (std::size_t i = 0; i < std::min(this->m_k, this->m_data.size()); ++i)
        {
            centroids.push_back(CentroidPoint<PT, PD>(this->m_data[i].coordinates));
        }
    }
};

class CentroidInitMethodTest : public ::testing::Test
{
protected:
    std::vector<Point<double, 2>> points2D = {
        Point<double, 2>({1.2345, 2.6789}, -1),
        Point<double, 2>({3.4567, 4.8910}, -1),
        Point<double, 2>({5.6789, 6.1234}, -1)};

    std::vector<Point<double, 3>> points3D = {
        Point<double, 3>({1.2345, 2.6789, 3.9876}, -1),
        Point<double, 3>({4.1234, 5.5678, 6.7890}, -1),
        Point<double, 3>({7.4567, 8.8910, 9.2345}, -1)};
};

// Test constructor initialization with data only
TEST_F(CentroidInitMethodTest, Constructor_WithData)
{
    ConcreteCentroidInit<double, 2> method(points2D);
    EXPECT_EQ(method.get_k(), 0); // Default k should be 0
}

// Test constructor initialization with data and k
TEST_F(CentroidInitMethodTest, Constructor_WithDataAndK)
{
    ConcreteCentroidInit<double, 2> method(points2D, 3);
    EXPECT_EQ(method.get_k(), 3);
}

// Test truncateToThreeDecimals
TEST_F(CentroidInitMethodTest, TruncateToThreeDecimals)
{
    ConcreteCentroidInit<double, 2> method(points2D);
    EXPECT_DOUBLE_EQ(method.truncateToThreeDecimals(1.23456), 1.234);
    EXPECT_DOUBLE_EQ(method.truncateToThreeDecimals(2.67894), 2.678);
    EXPECT_DOUBLE_EQ(method.truncateToThreeDecimals(9.99999), 9.999);
}

// Test exportedMesh (2D points)
TEST_F(CentroidInitMethodTest, ExportedMesh2D)
{
    ConcreteCentroidInit<double, 2> method(points2D);
    std::string filename = "test_output_2D";
    method.exportedMesh(points2D, filename);

    std::ifstream file(filename + ".csv");
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line); // Read header
    EXPECT_EQ(line, "x,y,label");

    std::getline(file, line);
    EXPECT_EQ(line, "1.234,2.678,0");

    std::getline(file, line);
    EXPECT_EQ(line, "3.456,4.891,0");

    std::getline(file, line);
    EXPECT_EQ(line, "5.678,6.123,0");

    file.close();
    std::remove((filename + ".csv").c_str()); // Cleanup
}

// Test exportedMesh (3D points)
TEST_F(CentroidInitMethodTest, ExportedMesh3D)
{
    ConcreteCentroidInit<double, 3> method(points3D);
    std::string filename = "test_output_3D";
    method.exportedMesh(points3D, filename);

    std::ifstream file(filename + ".csv");
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line); // Read header
    EXPECT_EQ(line, "x,y,z,label");

    std::getline(file, line);
    EXPECT_EQ(line, "1.234,2.678,3.987,0");

    std::getline(file, line);
    EXPECT_EQ(line, "4.123,5.567,6.789,0");

    std::getline(file, line);
    EXPECT_EQ(line, "7.456,8.891,9.234,0");

    file.close();
    std::remove((filename + ".csv").c_str()); // Cleanup
}
