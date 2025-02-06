#ifndef KDE_HPP
#define KDE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include <stdexcept>
#include <cstddef>

#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"
#include "clustering/CentroidInitializationMethods/MostDistantCentroids.hpp"
#include "clustering/CentroidInitializationMethods/KernelFunction.hpp"

#define NUMBER_RAY_STEP 3

using namespace Eigen;

template <typename PT, std::size_t PD>
class Point;

template <typename PT, std::size_t PD>
class CentroidPoint;

template<std::size_t PD>
class KDE : public CentroidInitMethod<double, PD> {
public:
    // Constructor with k
    KDE(std::vector<Point<double, PD>>& data, int k);

    // Constructor without k
    KDE(std::vector<Point<double, PD>>& data);

    void findCentroid(std::vector<CentroidPoint<double, PD>>& centroids) override;

    int findLocalWithoutRestriction();

private:
    int m_ray;
    int m_bandwidthMethods;
    int range_number_division;
    std::size_t m_totalPoints;
    double m_h_det_sqrt;
    Eigen::MatrixXd m_h;
    MatrixXd m_h_sqrt_inv;
    std::vector<Eigen::VectorXd> m_transformedPoints;
    std::vector<double> m_rs;
    std::vector<double> m_range;
    std::vector<double> m_step;

    // Mean and standard deviation
    std::pair<double, double> computeMeanAndStdDev(int dim);

    // Bandwidth matrix calculation
    Eigen::MatrixXd bandwidth_RuleOfThumb();

    // KDE value calculation
    double kdeValue(const Point<double, PD>& x);

    // Convert Point to VectorXd
    Eigen::VectorXd pointToVector(const Point<double, PD>& point);

    // Generate grid
    std::vector<Point<double, PD>> generateGrid();

    // Find local maxima
    void findLocalMaxima(const std::vector<Point<double, PD>>& gridPoints, std::vector<CentroidPoint<double, PD>>& returnVec);

    // Check local maximum
    bool isLocalMaximum(const std::vector<Point<double, PD>>& gridPoints, const std::vector<double>& densities, size_t index, std::vector<double> offsets);

    // Generate neighbors
    void generateNeighbors(const std::vector<Point<double, PD>>& grid, const Point<double, PD>& currentPoint, int dim, std::vector<size_t>& neighbors, std::vector<double>& offsets);

};


#endif
