#ifndef RANDOM_CENTROID_INIT_HPP
#define RANDOM_CENTROID_INIT_HPP

#include "geometry/point/CentroidPoint.hpp"
#include "geometry/point/Point.hpp"
#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"
#include <random>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <numeric>

/**
 * \class RandomCentroidInit
 * \brief Implements a centroid initialization method using random selection.
 *
 * This class randomly selects k points from the dataset to serve as initial centroids.
 *
 * \tparam PT Type of the points (e.g., float, double, etc.).
 * \tparam PD Dimension of the data points.
 */
template <typename PT, std::size_t PD>
class RandomCentroidInit : public CentroidInitMethod<PT, PD> {
public:
    /**
     * \brief Constructor: Initializes centroids randomly from the dataset.
     * \param data The dataset from which to select centroids.
     * \param k The number of centroids to initialize.
     */
    RandomCentroidInit(const std::vector<Point<PT, PD>>& data, int k);

    /**
     * \brief Constructor: Initializes centroids randomly from the dataset with default k.
     * \param data The dataset from which to select centroids.
     */
    RandomCentroidInit(const std::vector<Point<PT, PD>>& data);

    /**
     * \brief Finds and assigns centroids using random selection.
     * \param centroids The vector where the selected centroids will be stored.
     */
    void findCentroid(std::vector<CentroidPoint<PT, PD>>& centroids) override;
};

#endif // RANDOM_CENTROID_INIT_HPP
