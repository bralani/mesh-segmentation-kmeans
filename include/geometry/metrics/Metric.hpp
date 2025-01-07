#ifndef METRICS_HPP
#define METRICS_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>

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
};


#endif