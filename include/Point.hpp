#ifndef POINT_HPP
#define POINT_HPP

#include <array>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include "typedefs.h"

/**
 * Template class to represent a generic point in PD-dimensional space.
 * 
 * Template parameters:
 * - PT: The type of the coordinate coordinates (e.g., float, double, int).
 * - PD: The number of dimensions of the point.
 */
// Aggiungere il supporto per somma, sottrazione e divisione con uno scalare
template <typename PT, std::size_t PD>
class Point {
public:
    std::array<PT, PD> coordinates; // Coordinate del punto

    Point(){
        coordinates.fill(PT(0));
    }

    Point(const std::array<PT, PD>& coords) : coordinates(coords){}

    Point(PT value)  {
        coordinates.fill(value);
    }

    constexpr std::size_t getDimensions() const {
        return PD;
    }

    const std::array<PT, PD>& getValues() const {
        return coordinates;
    }

    void setValue(PT value, int idx) {
        if (idx < 0 || idx >= static_cast<int>(PD)) {
            throw std::out_of_range("Index out of bounds");
        }
        coordinates[idx] = value;
    }

    // Operatore di somma
    Point<PT, PD> operator+(const Point<PT, PD>& other) const {
        Point<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[i] + other.coordinates[i];
        }
        return result;
    }

    // Operatore di sottrazione
    Point<PT, PD> operator-(const Point<PT, PD>& other) const {
        Point<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[i] - other.coordinates[i];
        }
        return result;
    }

    // Operatore di divisione per uno scalare
    Point<PT, PD> operator/(PT scalar) const {
        if (scalar == PT(0)) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
        Point<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[i] / scalar;
        }
        return result;
    }

    /**
     * Calculates the distance between this point and another point using a given metric
     * @param other Another point
     * @param metric A function that calculates the distance between two arrays of coordinates
     * @return The distance between the two points
     */
    PT distanceTo(const Point<PT, PD>& other, const DistanceMetric& metric) const {
        //return metric(coordinates, other.getValues(), PD);
        PT sum = PT(0);
        for (std::size_t i = 0; i < PD; ++i) {
            PT diff = coordinates[i] - other.coordinates[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }

    // Static function to compute the sum of a vector of points
    static Point<PT, PD> vectorSum(const std::vector<Point<PT, PD>>& points) {
        if (points.empty()) {
            throw std::invalid_argument("The vector of points must not be empty.");
        }

        Point<PT, PD> sum;
        for (const auto& point : points) {
            sum = sum + point;
        }
        return sum;
    }

    void print() const {
        std::cout << "(";
        for (std::size_t i = 0; i < PD; ++i) {
            std::cout << coordinates[i];
            if (i < PD - 1) std::cout << ", ";
        }
        std::cout << ")";
    }

    
};


#endif
