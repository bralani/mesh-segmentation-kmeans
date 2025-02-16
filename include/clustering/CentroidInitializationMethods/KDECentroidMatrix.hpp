#ifndef KDE3D_HPP
#define KDE3D_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include <stdexcept>
#include <fstream>
#include <cstddef>
#include "clustering/CentroidInitializationMethods/KernelFunction.hpp"
#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"
#include "clustering/CentroidInitializationMethods/MostDistantCentroids.hpp"

#define PDS 3
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using Grid3D = std::vector<std::vector<std::vector<Point<double, 3>>>>;
using Densities3D = std::vector<std::vector<std::vector<double>>>;
using namespace Eigen;

template <typename PT, std::size_t PD>
class Point;

template <typename PT, std::size_t PD>
class CentroidPoint;

class KDE3D : public CentroidInitMethod<double, 3> {
public:
    KDE3D(const std::vector<Point<double, 3>>& data, int k);
    KDE3D(const std::vector<Point<double, 3>>& data);

    void findCentroid(std::vector<CentroidPoint<double, 3>>& centroids) override;

private:
    double truncateToThreeDecimals(double value);
    int m_bandwidthMethods;
    int range_number_division;
    std::size_t m_totalPoints;
    double m_h_det_sqrt;
    Eigen::MatrixXd m_h;
    MatrixXd m_h_sqrt_inv;
    std::vector<Eigen::VectorXd> m_transformedPoints;
    std::array<double, 3> m_range;
    std::array<double, 3> m_step;
    std::array<size_t, 3> m_numPoints;

    // Mean and standard deviation
    std::pair<double, double> computeMeanAndStdDev(int dim);

    // Bandwidth matrix calculation
    Eigen::MatrixXd bandwidth_RuleOfThumb();

    // KDE3D value calculation
    double kdeValue(const Point<double, 3>& x);

    // Convert Point to VectorXd
    Eigen::VectorXd pointToVector(const Point<double, 3>& point);

    // Generate grid
    Grid3D generateGrid();

    // Find local maxima in the grid
    void findLocalMaxima(const Grid3D& gridPoints, std::vector<CentroidPoint<double, PDS>>& returnVec);

    // Check if a point is a local maximum
    bool isLocalMaximum(const Grid3D& gridPoints,const Densities3D& densities, size_t x, size_t y, size_t z);
};

#endif // KDE3D_HPP
