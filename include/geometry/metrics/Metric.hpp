#ifndef METRICS_HPP
#define METRICS_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <optional>

#include "geometry/point/CentroidPoint.hpp"
#include "geometry/point/Point.hpp"

/**
 * \class Metric
 * \brief Abstract class representing a generic metric for calculating distances.
 * 
 * This abstract class serves as a base for implementing various distance metrics, 
 * such as Euclidean or geodesic distances. It provides common functionality for 
 * setting up and working with centroids and points. Derived classes must implement 
 * the `setup` and `fit_cpu` (and optionally `fit_gpu`) methods.
 * 
 * \tparam PT Type of the point (e.g., float, double)
 * \tparam PD Dimension of the point (e.g., 3D)
 */
template <typename PT, std::size_t PD>
class Metric
{
public:
    /**
     * \brief Default constructor that initializes centroids to null.
     * 
     * This constructor initializes the metric with no centroids. The centroids 
     * must be set later using the `setCentroids` method.
     */
    Metric();

    /**
     * \brief Constructor to initialize centroids reference.
     * 
     * This constructor initializes the metric with the given reference to centroids.
     * 
     * \param centroids A reference to a vector of centroid points.
     */
    explicit Metric(std::vector<CentroidPoint<PT, PD>> &centroids);

    /**
     * \brief Constructor to initialize centroids reference and data.
     * 
     * This constructor initializes the metric with both centroids and data points.
     * 
     * \param centroids A reference to a vector of centroid points.
     * \param data A vector of data points used for the metric calculations.
     */
    explicit Metric(std::vector<CentroidPoint<PT, PD>> &centroids, std::vector<Point<PT, PD>> data);

    /**
     * \brief Virtual destructor to ensure proper cleanup in derived classes.
     * 
     * This destructor ensures that any resources allocated in derived classes are 
     * properly cleaned up when an object of a derived class is destroyed.
     */
    virtual ~Metric();

    /**
     * \brief Virtual function to setup the metric.
     * 
     * This method must be implemented in derived classes to perform any necessary 
     * setup before fitting or calculating the metric.
     */
    virtual void setup() = 0;

    /**
     * \brief Sets the centroids for the metric.
     * 
     * This method sets the centroids used in the metric calculation.
     * 
     * \param centroids A reference to a vector of centroid points.
     */
    void setCentroids(std::vector<CentroidPoint<PT, PD>> &centroids);

    /**
     * \brief Fits the KMeans algorithm on the CPU.
     * 
     * This method must be implemented in derived classes to compute the fitting 
     * of the KMeans algorithm on the CPU. It can be used for clustering tasks.
     */
    virtual void fit_cpu() = 0;

    /**
     * \brief Sets the data points for the metric.
     * 
     * This method sets the data points used in the metric calculation.
     * 
     * \param data A vector of data points.
     */
    void setPoints(std::vector<Point<PT, PD>> data);

    /**
     * \brief Gets the data points used for the metric calculations.
     * 
     * \return A reference to the vector of data points.
     */
    virtual std::vector<Point<PT, PD>>& getPoints() = 0;

    #ifdef USE_CUDA
    /**
     * \brief Fits the KMeans algorithm on the GPU.
     * 
     * This method must be implemented in derived classes to compute the fitting 
     * of the KMeans algorithm on the GPU. It can be used for faster clustering 
     * tasks, utilizing CUDA kernels.
     */
    virtual void fit_gpu() = 0;
    #endif

    /**
     * \brief Resets the centroids to their initial state.
     * 
     * This method resets the centroids to their original values.
     */
    void resetCentroids();

protected:
    double threshold; /**< A threshold value used in the metric calculation. */
    std::vector<CentroidPoint<PT, PD>> oldCentroids; /**< Stores the old centroids for comparison. */
    std::vector<CentroidPoint<PT, PD>> *centroids; /**< Pointer to the vector of centroids. */
    std::vector<Point<PT, PD>> data; /**< Stores the data points used in the metric calculation. */

    /**
     * \brief Stores the centroids after the fitting process.
     * 
     * This method stores the centroids after a fitting process (e.g., KMeans).
     * 
     * This method must be implemented in derived classes to perform the actual 
     * storage of centroids.
     */
    virtual void storeCentroids() = 0;
};

#endif
