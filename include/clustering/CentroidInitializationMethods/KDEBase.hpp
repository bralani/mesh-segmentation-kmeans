#ifndef KDEBASE_HPP
#define KDEBASE_HPP

#include <vector>
#include <Eigen/Dense>
#include "geometry/point/Point.hpp"
#include "geometry/point/CentroidPoint.hpp"
#include "clustering/CentroidInitializationMethods/KernelFunction.hpp"

#define RAY_MIN 3
#define RANGE_MIN 9

/**
 * \class KDEBase
 * \brief Base class for Kernel Density Estimation (KDE) operations.
 *
 * This class provides fundamental methods for KDE computations, including bandwidth estimation,
 * mean and standard deviation calculations, and KDE value evaluation at a given point.
 *
 * \tparam PD Dimension of the data points.
 */
template<std::size_t PD>
class KDEBase {
public:
    /**
     * \brief Default constructor.
     */
    KDEBase() = default;

    /**
     * \brief Default destructor.
     */
    ~KDEBase() = default;

    /**
     * \brief Computes the mean and standard deviation for a given dimension.
     *
     * \param dim The dimension for which statistics are computed.
     * \param m_data The dataset used for the computation.
     * \return A pair containing the mean and standard deviation.
     */
    std::pair<double, double> computeMeanAndStdDev(int dim, const std::vector<Point<double, PD>>& m_data);

    /**
     * \brief Computes the bandwidth matrix using the Rule of Thumb method.
     *
     * This method estimates the bandwidth matrix required for KDE calculations.
     *
     * \param m_data The dataset used for bandwidth estimation.
     * \return The computed bandwidth matrix.
     */
    Eigen::MatrixXd bandwidth_RuleOfThumb(const std::vector<Point<double, PD>>& m_data);

    /**
     * \brief Computes the KDE value at a given point.
     *
     * This method evaluates the KDE function at a specific point in the dataset.
     *
     * \param x The point at which KDE is computed.
     * \return The estimated density value at the given point.
     */
    double kdeValue(const Point<double, PD>& x);

    /**
     * \brief Converts a Point object to an Eigen vector.
     *
     * \param point The point to be converted.
     * \return An Eigen::VectorXd representation of the input point.
     */
    Eigen::VectorXd pointToVector(const Point<double, PD>& point);

    std::vector<Eigen::VectorXd> m_transformedPoints; ///< Transformed points for KDE calculations.
    Eigen::MatrixXd m_h_sqrt_inv; ///< Inverse square root of the bandwidth matrix.
    double m_h_det_sqrt; ///< Square root of the determinant of the bandwidth matrix.
    Eigen::MatrixXd m_h; ///< Bandwidth matrix used for KDE computations.
};

#endif // KDEBASE_HPP
