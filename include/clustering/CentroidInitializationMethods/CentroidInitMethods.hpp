#ifndef CENTROID_INIT_METHOD_HPP
#define CENTROID_INIT_METHOD_HPP

#include <vector>
#include <cstddef>
#include "geometry/point/Point.hpp"
#include "geometry/point/CentroidPoint.hpp"

/**
 * @class CentroidInitMethod
 * @brief A class for managing the initialization of centroids and computing cluster centroids.
 */
template <typename PT, std::size_t PD>
class CentroidInitMethod {
public:
    // Virtual destructor
    virtual ~CentroidInitMethod() = default;

    /**
     * @brief Constructor: Initialize centroids using the dataset.
     * @param data The dataset from which to initialize centroids.
     */
    explicit CentroidInitMethod(std::vector<Point<PT, PD>>& data);

    /**
     * @brief Constructor: Initialize centroids using the dataset and the number of clusters.
     * @param data The dataset from which to initialize centroids.
     * @param k The number of centroids to initialize.
     */
    explicit CentroidInitMethod(std::vector<Point<PT, PD>>& data, int k);

    /**
     * @brief Abstract method to find a centroid given a set of centroids.
     * @param centroids The centroids to process.
     */
    virtual void findCentroid(std::vector<CentroidPoint<PT, PD>>& centroids) = 0;

protected:
    std::vector<Point<PT, PD>> m_data; ///< Dataset
    int m_k = 0;                       ///< Number of clusters

    // Setter for `m_k`
    void set_k(int k);
};


#endif // CENTROID_INIT_METHOD_HPP
