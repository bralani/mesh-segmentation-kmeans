#ifndef POINT_HPP
#define POINT_HPP

#include <array>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <vector>
#include <numeric>

template <typename PT, std::size_t PD>
class Point {
public:
    std::array<PT, PD> coordinates;                     // Array to store the coordinates of the point
    int id;                                             // Unique identifier for the point
    std::shared_ptr<Point<PT, PD>> centroid = nullptr;  // Optional pointer to the centroid of the point (if applicable)
    
    Point();  // Default constructor
    Point(const std::array<PT, PD>& coords, int id = -1);  // Constructor with array of coordinates
    Point(PT value, int id = -1);  // Constructor to initialize all coordinates with a single value

    constexpr std::size_t getDimensions() const;  // Returns the number of dimensions
    const std::array<PT, PD>& getValues() const;  // Returns the coordinates
    void setValue(PT value, int idx);  // Sets a specific coordinate
    void setCentroid(std::shared_ptr<Point<PT, PD>> centroid);  // Sets the centroid
    void setID(int id);  // Sets the ID of the point

    bool operator==(const Point& other) const;  // Equality operator
    virtual Point<PT, PD> operator+(const Point<PT, PD>& other) const;  // Addition operator
    Point<PT, PD> operator-(const Point<PT, PD>& other) const;  // Subtraction operator
    Point<PT, PD> operator/(const Point<PT, PD>& other) const;  // Division operator

    static Point<PT, PD> vectorSum(typename std::vector<Point<PT, PD>>::iterator begin,
                                   typename std::vector<Point<PT, PD>>::iterator end);

    Point<PT, PD> cross(const Point<PT, PD>& other) const;  // Cross product
    PT norm() const;  // Norm of the point

    void print() const;  // Print coordinates
    friend std::ostream& operator<<(std::ostream& os, const Point<PT, PD>& point) {
        os << "(";
        for (std::size_t i = 0; i < PD; ++i) {
            os << point.coordinates[i];
            if (i < PD - 1) os << ", ";
        }
        os << ")";
        return os;
    }

    virtual ~Point() = default;  // Virtual destructor
};

#endif
