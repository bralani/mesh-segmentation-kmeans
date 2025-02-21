#ifndef KDEBASE_HPP
#define KDEBASE_HPP

#include <vector>
#include <Eigen/Dense>
#include "geometry/point/Point.hpp"
#include "geometry/point/CentroidPoint.hpp"
#include "clustering/CentroidInitializationMethods/KernelFunction.hpp"

#define RAY_MIN 3
#define RANGE_MIN 9

template<std::size_t PD>
class KDEBase {
public:
    // Constructor and destructor
    KDEBase() = default;
    ~KDEBase() = default;

    // Method to compute mean and standard deviation for a given dimension
    std::pair<double, double> computeMeanAndStdDev(int dim,const std::vector<Point<double, PD>>& m_data);

    // Method to compute the bandwidth matrix using the Rule of Thumb method
    Eigen::MatrixXd bandwidth_RuleOfThumb(const std::vector<Point<double, PD>>& m_data);

    // Method to compute the KDE value at a given point
    double kdeValue(const Point<double, PD>& x);

    Eigen::VectorXd pointToVector(const Point<double, PD>& point);

    std::vector<Eigen::VectorXd> m_transformedPoints;
    Eigen::MatrixXd m_h_sqrt_inv;
    double m_h_det_sqrt;
    Eigen::MatrixXd m_h;
};

#endif // KDEBASE_HPP
