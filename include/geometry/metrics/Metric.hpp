#ifndef METRICS_HPP
#define METRICS_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <optional>
#include "geometry/point/CentroidPoint.hpp"

/**
 * Abstract class representing a generic metric for calculating distances.
 */
template <typename PT, std::size_t PD>
class Metric
{
public:
    // Default constructor with null centroids
    Metric() : centroids(nullptr) {}

    // Constructor to initialize centroids reference
    explicit Metric(std::vector<CentroidPoint<PT, PD>> &centroids) : centroids(&centroids) {}

public:

    // Virtual destructor to ensure proper cleanup in derived classes
    virtual ~Metric() = default;

    // Virtual function to setup the metric
    virtual void setup() = 0;

    // Set the centroids for the metric
    void setCentroids(std::vector<CentroidPoint<PT, PD>> &centroids) {
        this->centroids = &centroids;
    }

    // Fit the KMeans algorithm on the CPU or GPU
    virtual void fit_cpu() = 0;

    #ifdef USE_CUDA
        virtual void fit_gpu() = 0;
    #endif

protected:
    double threshold;
    std::vector<CentroidPoint<PT, PD>> oldCentroids;
    std::vector<CentroidPoint<PT, PD>> *centroids; // Pointer to centroids
};

#endif
