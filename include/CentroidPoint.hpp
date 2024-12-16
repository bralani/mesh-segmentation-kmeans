#ifndef CENTROID_POINT_HPP
#define CENTROID_POINT_HPP

#include "Point.hpp"
#include "HasWgtCent.hpp"
#include <iostream>
#include <array>

// The CentroidPoint class inherits from both Point and HasWgtCent
// This class represents a point with additional weighted centroids functionality
// It combines the properties of a point (coordinates) and weights (wgtCent) with a counter (count)
template <typename PT, std::size_t PD>
class CentroidPoint : public Point<PT, PD>, public HasWgtCent<PT, PD> {
public:
    // Default constructor
    // Initializes both Point and HasWgtCent part of the class
    CentroidPoint() 
        : Point<PT, PD>(), HasWgtCent<PT, PD>() {}

    // Constructor with a Point as input
    // Initializes the Point part with the given point and the HasWgtCent part as a default object
    CentroidPoint(const Point<PT, PD>& point)
        : Point<PT, PD>(point), HasWgtCent<PT, PD>() {}

    // Overloaded operator to add another HasWgtCent object to this CentroidPoint
    // This adds the weights and the count from another object to the current object
    CentroidPoint<PT, PD> operator+(const HasWgtCent<PT, PD>& other) const {
        CentroidPoint<PT, PD> result(*this); // Copy the current object
        for (std::size_t i = 0; i < PD; ++i) {
            result.wgtCent[i] = this->wgtCent[i] + other.wgtCent[i]; // Add the weights element-wise
        }
        result.count = this->count + other.count; // Add the count
        return result;
    }

    // Normalize the coordinates of the point based on the weights and the count
    // This will divide each coordinate by the count to calculate the centroid
    void normalize() {
        for (std::size_t i = 0; i < PD; ++i) {
            this->coordinates[i] = this->wgtCent[i] / this->count; // Set the coordinate as the weighted average
        }
    }

    // Virtual destructor
    // Ensures proper cleanup if the object is derived from or has dynamic resources
    virtual ~CentroidPoint() = default;
};

#endif // CENTROID_POINT_HPP
