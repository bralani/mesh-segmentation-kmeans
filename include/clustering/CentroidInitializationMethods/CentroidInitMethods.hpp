#ifndef CENTROID_INIT_METHOD_HPP
#define CENTROID_INIT_METHOD_HPP

#include <vector>
#include <stdexcept>
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
     * @brief Initialize centroids using k points from the dataset.
     * 
     * @param data The dataset from which to initialize centroids.
     * @param k The number of centroids to initialize.
     * @return A vector of initialized centroids.
     */
    explicit CentroidInitMethod(std::vector<Point<PT, PD>>& data) : m_data(data) {} ;

    /**
     * @brief Initialize centroids using the entire dataset.
     * 
     * @param data The dataset from which to initialize centroids.
     * @return A vector of initialized centroids.
     */
    explicit CentroidInitMethod(std::vector<Point<PT, PD>>& data, int k) : m_data(data), m_k(k) {};
    /**
     * @brief Abstract method to find a centroid given a set of centroids.
     * 
     * @param centroids The centroids to process.
     * @return The calculated centroid.
     */
    virtual void findCentroid(std::vector<CentroidPoint<PT, PD>>& centroids) = 0;

protected:
    std::vector<Point<PT, PD>> m_data; // Dataset
    int m_k = 0;                       // Number of clusters

    // Setter for `m_k`
    void set_k(int k) {
        m_k = k;
    }
};

#endif // CENTROID_INIT_METHOD_HPP
