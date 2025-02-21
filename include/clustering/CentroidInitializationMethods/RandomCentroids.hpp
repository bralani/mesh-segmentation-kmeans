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
 * @class RandomCentroidInit
 * @brief A class for initializing centroids randomly from the dataset.
 * 
 * This class implements the CentroidInitMethod interface and provides functionality
 * to select centroids randomly from the given dataset. It also includes methods to
 * calculate the centroid of a cluster.
 */
template <typename PT, std::size_t PD>
class RandomCentroidInit : public CentroidInitMethod<PT, PD> {
public:
    // Constructor with both data and the number of clusters
    RandomCentroidInit(const std::vector<Point<PT, PD>>& data, int k);

    // Constructor with only data; the number of clusters will be determined later
    RandomCentroidInit(const std::vector<Point<PT, PD>>& data);

    // Method to randomly find initial centroids
    void findCentroid(std::vector<CentroidPoint<PT, PD>>& centroids) override;
};


#endif
