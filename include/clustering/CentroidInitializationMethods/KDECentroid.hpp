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
#include "clustering/CentroidInitializationMethods/KDEBase.hpp"

using namespace Eigen;

/**
 * \class KDE
 * \brief Implements Kernel Density Estimation (KDE) for centroid initialization.
 *
 * This class derives from CentroidInitMethod and KDEBase to provide KDE-based
 * centroid initialization for clustering algorithms.
 *
 * \tparam PD Dimension of the data points.
 */
template <std::size_t PD>
class KDE : public CentroidInitMethod<double, PD>, public KDEBase<PD>
{
public:
    /**
     * \brief Constructor for KDE with specified number of centroids.
     *
     * Initializes KDE using a dataset and a predefined number of clusters.
     *
     * \param data The input dataset as a vector of points.
     * \param k Number of centroids to initialize.
     */
    KDE(const std::vector<Point<double, PD>> &data, int k);

    /**
     * \brief Constructor for KDE without specifying k.
     *
     * Uses an alternative initialization method where k is determined dynamically.
     *
     * \param data The input dataset as a vector of points.
     */
    KDE(const std::vector<Point<double, PD>> &data);

    /**
     * \brief Computes centroids using KDE-based initialization.
     *
     * Overrides the findCentroid method to calculate centroids based on KDE analysis.
     *
     * \param centroids Reference to a vector where the computed centroids will be stored.
     */
    void findCentroid(std::vector<CentroidPoint<double, PD>> &centroids) override;

    /**
     * \brief Finds local maxima without restriction.
     *
     * This method identifies local maxima in the KDE process without applying constraints.
     *
     * \return The number of local maxima found.
     */
    int findLocalWithoutRestriction();

    /**
     * \brief Generates a grid of points for KDE computation.
     *
     * The grid is used to estimate density values across the space.
     *
     * \return A vector of generated grid points.
     */
    std::vector<Point<double, PD>> generateGrid();

    /**
     * \brief Checks if a given point is a local maximum.
     *
     * Determines whether a point is a local maximum by comparing its KDE value with neighboring points.
     *
     * \param gridPoints The set of grid points.
     * \param densities The KDE values associated with each grid point.
     * \param index The index of the point to check.
     * \param neighborsParallel Reference to a vector storing neighbor indices.
     * \param offsetsParallel Reference to a vector storing neighbor offsets.
     * \return True if the point is a local maximum, false otherwise.
     */
    bool isLocalMaximum(const std::vector<Point<double, PD>> &gridPoints, const std::vector<double> &densities, size_t index, std::vector<std::size_t> &neighborsParallel, std::vector<double> &offsetsParallel);

private:
    int m_ray;                   ///< Radius for local maxima search.
    int m_bandwidthMethods;      ///< Method used for bandwidth selection.
    int range_number_division;   ///< Number of divisions for the range calculation.
    std::size_t m_totalPoints;   ///< Total number of points in the dataset.
    std::vector<double> m_rs;    ///< Vector containing radius values for KDE calculations.
    std::vector<double> m_range; ///< Range of KDE computations.
    std::vector<double> m_step;  ///< Step sizes used in KDE calculations.

    /**
     * \brief Finds local maxima in the KDE result.
     *
     * Uses the computed KDE values to detect peaks corresponding to potential centroids.
     *
     * \param gridPoints The points forming the KDE evaluation grid.
     * \param returnVec Reference to a vector where detected centroids will be stored.
     */
    void findLocalMaxima(const std::vector<Point<double, PD>> &gridPoints, std::vector<CentroidPoint<double, PD>> &returnVec);

    /**
     * \brief Generates neighboring points for a given grid point.
     *
     * Determines neighboring points in the grid to assist in local maximum detection.
     *
     * \param grid The set of grid points.
     * \param currentPoint The point for which neighbors are being generated.
     * \param dim The dimensionality of the space.
     * \param neighbors Reference to a vector storing indices of neighboring points.
     * \param offsets Reference to a vector storing offsets of neighboring points.
     */
    void generateNeighbors(const std::vector<Point<double, PD>> &grid, const Point<double, PD> &currentPoint, int dim, std::vector<size_t> &neighbors, std::vector<double> &offsets);
};

#endif