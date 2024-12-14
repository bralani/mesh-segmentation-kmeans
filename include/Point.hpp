#ifndef POINT_HPP
#define POINT_HPP

#include <array>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <numeric>

/**
 * Template class to represent a generic point in PD-dimensional space.
 * 
 * Template parameters:
 * - PT: The type of the coordinate coordinates (e.g., float, double, int).
 * - PD: The number of dimensions of the point.
 */
template <typename PT, std::size_t PD>
class Point {
public:
    std::array<PT, PD> coordinates; // Coordinates of the point

    // Default constructor: initializes all coordinates to zero
    Point() {
        coordinates.fill(PT(0));
    }

    // Constructor that accepts an array of coordinates
    explicit Point(const std::array<PT, PD>& coords) : coordinates(coords) {}

    // Constructor that accepts an initial value for all coordinates
    explicit Point(PT value) {
        coordinates.fill(value);
    }

    /**
     * Getter for the number of dimensions
     * @return The number of dimensions (always PD)
     */
    constexpr std::size_t getDimensions() const {
        return PD;
    }

    /**
     * Getter for coordinate coordinates
     * @return A constant reference to the array of coordinate coordinates
     */
    const std::array<PT, PD>& getValues() const {
        return coordinates;
    }

    /**
     * Setter for coordinate coordinates
     * @param newValues The new coordinate coordinates
     */
    void setValues(const std::array<PT, PD>& newValues) {
        coordinates = newValues;
    }


    /**
     * Calculates the distance between this point and another point using a given metric
     * @param other Another point
     * @param metric A function that calculates the distance between two arrays of coordinates
     * @return The distance between the two points
     */
/*
    double distanceTo(const Point<PT, PD>& other, const DistanceMetric<PT>& metric) const {
        return metric(coordinates, other.getValues());
    }
*/
    // Old implementation (remove it when no longer needed)
    /*// Method to calculate the Euclidean distance to another point
    PT distanceTo(const Point<PT, PD>& other) const {
        PT sum = PT(0);
        for (std::size_t i = 0; i < PD; ++i) {
            PT diff = coordinates[i] - other.coordinates[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }*/

    // Addition operator (+)
    Point<PT, PD> operator+(const Point<PT, PD>& other) const {
        Point<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = coordinates[i] + other.coordinates[i];
        }
        return result;
    }

    // Subtraction operator (-)
    Point<PT, PD> operator-(const Point<PT, PD>& other) const {
        Point<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = coordinates[i] - other.coordinates[i];
        }
        return result;
    }

    // Multiplication operator for a scalar (*)
    Point<PT, PD> operator*(PT scalar) const {
        Point<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = coordinates[i] * scalar;
        }
        return result;
    }

    // Method to print the point
    void print() const {
        std::cout << "(";
        for (std::size_t i = 0; i < PD; ++i) {
            std::cout << coordinates[i];
            if (i < PD - 1) std::cout << ", ";
        }
        std::cout << ")";
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

    /**
     * Overload of the << operator to print the Point
     * @param os The output stream
     * @param point The Point object to print
     * @return The output stream with the Point's string representation
     */
    friend std::ostream& operator<<(std::ostream& os, const Point<PT, PD>& point) {
        os << "Point(" << PD << "D: [";
        for (std::size_t i = 0; i < PD; ++i) {
            os << point.coordinates[i];
            if (i < PD - 1) {
                os << ", ";
            }
        }
        os << "])";
        return os;
    }
};

#endif
