#ifndef METRICS_HPP
#define METRICS_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include "geometry/point/CentroidPoint.hpp"

/**
 * Abstract class representing a generic metric for calculating distances.
 */
template <typename PT, std::size_t PD>
class Metric
{
public:
    // Pure virtual function to calculate distance
    virtual PT distanceTo(const Point<PT, PD>& a, const Point<PT, PD>& b) const = 0;

    // Virtual destructor to ensure proper cleanup in derived classes
    virtual ~Metric() = default;

    // Virtual function to setup the metric
    virtual void setup() = 0;

    // Virtual function to perform initial setup
    virtual void initialSetup() = 0;

    // Set the centroids for the metric
    virtual void setCentroids(std::vector<CentroidPoint<PT, PD>> &centroids) = 0;
};


#endif