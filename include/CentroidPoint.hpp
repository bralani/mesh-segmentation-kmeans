#ifndef CENTROID_POINT_HPP
#define CENTROID_POINT_HPP

#include "Point.hpp"
#include <iostream>
#include <array>

/**
 * Class template to represent a centroid point derived from Point
 * Template parameters:
 * - PT: The type of the coordinate values (e.g., double, float)
 * - PD: The number of dimensions of the point (e.g., 2 for 2D, 3 for 3D)
 */
template <typename PT, std::size_t PD>
class CentroidPoint : public Point<PT, PD> {
public:
    int count; // Number of points in the cell

    /**
     * Constructor with coordinates
     * @param coordinates The coordinate values for the centroid point
     */
    CentroidPoint(const std::array<PT, PD>& coordinates)
        : Point<PT, PD>(coordinates), count(0) {}

    /**
     * Constructor from a Point object
     * @param point The Point object to use as the base for this CentroidPoint
     */
    CentroidPoint(const Point<PT, PD>& point)
        : Point<PT, PD>(point), count(0) {}

    /**
     * Print the CentroidPoint, including additional information
     */
    void print() const {
        std::cout << "CentroidPoint (";
        for (std::size_t i = 0; i < PD; ++i) {
            std::cout << this->getValues()[i];
            if (i < PD - 1) std::cout << ", ";
        }
        std::cout << ") - Count: " << count << "\n";
    }
};

#endif // CENTROID_POINT_HPP
