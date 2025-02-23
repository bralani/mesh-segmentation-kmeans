/*============================================================================
 *  KDE3D.hpp
 *============================================================================*/

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
#include "clustering/CentroidInitializationMethods/KDEBase.hpp"

#define PDS 3
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using Grid3D = std::vector<std::vector<std::vector<Point<double, 3>>>>;
using Densities3D = std::vector<std::vector<std::vector<double>>>;
using namespace Eigen;

/**
 * \class KDE3D
 * \brief Implements Kernel Density Estimation (KDE) for 3D data.
 *
 * This class inherits from CentroidInitMethod and KDEBase to provide KDE-based
 * centroid initialization in three-dimensional space.
 */
class KDE3D : public CentroidInitMethod<double, 3>, public KDEBase<3>
{
public:
    /**
     * \brief Constructor for KDE3D.
     *
     * Initializes KDE3D with the given data and number of clusters.
     *
     * \param data Vector of 3D points used for density estimation.
     * \param k Number of centroids to determine.
     */
    KDE3D(const std::vector<Point<double, 3>> &data, int k);

    /**
     * \brief Constructor for KDE3D with a predefined number of centroids.
     *
     * \param data Vector of 3D points used for density estimation.
     */
    KDE3D(const std::vector<Point<double, 3>> &data);

    /**
     * \brief Finds initial centroids using KDE-based local maxima detection.
     *
     * \param centroids Vector where the detected centroids will be stored.
     */
    void findCentroid(std::vector<CentroidPoint<double, 3>> &centroids) override;

    /**
     * \brief Generates a 3D grid based on input data.
     *
     * \return A 3D grid of points for KDE calculations.
     */
    Grid3D generateGrid();

    /**
     * \brief Identifies local maxima in the KDE grid.
     *
     * \param gridPoints The generated grid of points.
     * \param returnVec Vector where detected centroids will be stored.
     */
    void findLocalMaxima(const Grid3D &gridPoints, std::vector<CentroidPoint<double, PDS>> &returnVec);

private:
    /**
     * \brief Truncates a value to three decimal places.
     *
     * \param value Input value.
     * \return Truncated value.
     */
    double truncateToThreeDecimals(double value);

    int m_bandwidthMethods;            ///< Stores bandwidth method selection.
    int range_number_division;         ///< Number of divisions for range computation.
    std::size_t m_totalPoints;         ///< Total number of data points.
    std::array<double, 3> m_range;     ///< Range of the 3D data space.
    std::array<double, 3> m_step;      ///< Step sizes for grid generation.
    std::array<size_t, 3> m_numPoints; ///< Number of points per axis in the grid.

    /**
     * \brief Checks if a point is a local maximum in the KDE density grid.
     *
     * \param gridPoints The generated grid of points.
     * \param densities Computed density values for the grid.
     * \param x X-coordinate index.
     * \param y Y-coordinate index.
     * \param z Z-coordinate index.
     * \return True if the point is a local maximum, false otherwise.
     */
    bool isLocalMaximum(const Grid3D &gridPoints, const Densities3D &densities, size_t x, size_t y, size_t z);
};

#endif // KDE3D_HPP
