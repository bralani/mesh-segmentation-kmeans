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
    int count; // Numero di punti nel cluster

    CentroidPoint() : Point<PT, PD>(), count(1) {}

    CentroidPoint(const std::array<PT, PD>& coordinates)
        : Point<PT, PD>(coordinates), count(1) {}

    CentroidPoint(const Point<PT, PD>& point)
        : Point<PT, PD>(point), count(1) {}

    // Operatore di somma
    CentroidPoint<PT, PD> operator+(const Point<PT, PD>& other) const {
        CentroidPoint<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[i] + other.coordinates[i];
        }
        result.count = this->count;
        return result;
    }

    // Operatore di sottrazione
    CentroidPoint<PT, PD> operator-(const Point<PT, PD>& other) const {
        CentroidPoint<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[i] - other.coordinates[i];
        }
        result.count = this->count;
        return result;
    }

    // Operatore di divisione per uno scalare
    CentroidPoint<PT, PD> operator/(PT scalar) const {
        if (scalar == PT(0)) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
        CentroidPoint<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[i] / scalar;
        }
        result.count = this->count;
        return result;
    }

    void print() const {
        std::cout << "CentroidPoint (";
        for (std::size_t i = 0; i < PD; ++i) {
            std::cout << this->coordinates[i];
            if (i < PD - 1) std::cout << ", ";
        }
        std::cout << ") - Count: " << count << "\n";
    }
};


#endif // CENTROID_POINT_HPP
