// metrics.hpp
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
 * Abstract class representing a generic metric for calculating distances.
 */
template <typename PT, std::size_t PD>
class Metric
{
public:
    // Default constructor with null centroids
    Metric();

    // Constructor to initialize centroids reference
    explicit Metric(std::vector<CentroidPoint<PT, PD>> &centroids);

    // Constructor to initialize centroids reference
    explicit Metric(std::vector<CentroidPoint<PT, PD>> &centroids, std::vector<Point<PT, PD>> data);

    // Virtual destructor to ensure proper cleanup in derived classes
    virtual ~Metric();

    // Virtual function to setup the metric
    virtual void setup() = 0;

    // Set the centroids for the metric
    void setCentroids(std::vector<CentroidPoint<PT, PD>> &centroids);

    // Fit the KMeans algorithm on the CPU or GPU
    virtual void fit_cpu() = 0;

    //set points 
    void setPoints(std::vector<Point<PT, PD>> data);

    //get points
    virtual std::vector<Point<PT, PD>>& getPoints() = 0;

    #ifdef USE_CUDA
        virtual void fit_gpu() = 0;
    #endif

    void resetCentroids();

protected:
    double threshold;
    std::vector<CentroidPoint<PT, PD>> oldCentroids;
    std::vector<CentroidPoint<PT, PD>> *centroids; // Pointer to centroids
    std::vector<Point<PT, PD>> data;

    virtual void storeCentroids() = 0 ;
};


#endif