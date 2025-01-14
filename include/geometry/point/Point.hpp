#ifndef POINT_HPP
#define POINT_HPP

#include <array>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <vector>
#include <numeric>

/* Class to represent a point in a space of arbitrary dimensions (PD)
* PT represents the type of the coordinates (e.g., double, int), and PD represents the number of dimensions
*/
template <typename PT, std::size_t PD>
class Point {
public:
    int id;                                             // Unique identifier for the point
    std::array<PT, PD> coordinates;                     // Array to store the coordinates of the point
    std::shared_ptr<Point<PT, PD>> centroid = nullptr;  // Optional pointer to the centroid of the point (if applicable)
    
    // Default constructor, initializes coordinates to zero
    Point(){
        coordinates.fill(PT(0));
        id = -1;
    }

    // Constructor to initialize with a specific array of coordinates
    Point(const std::array<PT, PD>& coords, int id = -1) : coordinates(coords), id(id) {}

    // Constructor to initialize all coordinates with a single value
    Point(PT value, int id = -1)  {
        coordinates.fill(value);
        this->id = id;
    }

    // Returns the number of dimensions of the point (PD)
    constexpr std::size_t getDimensions() const {
        return PD;
    }

    // Returns the coordinates of the point as a const reference
    const std::array<PT, PD>& getValues() const {
        return coordinates;
    }

    // Sets the value of a specific coordinate at index 'idx'
    // Throws an exception if 'idx' is out of bounds
    void setValue(PT value, int idx) {
        if (idx < 0 || idx >= static_cast<int>(PD)) {
            throw std::out_of_range("Index out of bounds");
        }
        coordinates[idx] = value;
    }

    // Sets the centroid of this point
    void setCentroid(std::shared_ptr<Point<PT, PD>> centroid){
        this->centroid = centroid;
    }

    // Operatore equivalenza con tollernza
    bool operator==(const Point& other) const {
        for (std::size_t i = 0; i < PD; ++i) {
            if (std::abs(coordinates[i] - other.coordinates[i]) > 1e-6) { 
                return false;
            }
        }
        return true;
    }

    // Overloaded operator for adding two points (coordinate-wise addition)
    virtual Point<PT, PD> operator+(const Point<PT, PD>& other) const {
        Point<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[i] + other.coordinates[i];
        }
        return result;
    }

    // Overloaded operator for subtracting two points (coordinate-wise subtraction)
    Point<PT, PD> operator-(const Point<PT, PD>& other) const {
        Point<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[i] - other.coordinates[i];
        }
        return result;
    }

    // Overloaded operator for dividing two points (coordinate-wise subtraction)
    Point<PT, PD> operator/(const Point<PT, PD>& other) const {
        Point<PT, PD> result;
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[i] / other.coordinates[i];
        }
        return result;
    }

    // Static function to compute the sum of a vector of points (element-wise)
    static Point<PT, PD> vectorSum(typename std::vector<Point<PT, PD>>::iterator begin,
                                   typename std::vector<Point<PT, PD>>::iterator end) {
        Point<PT, PD> sum;
        for (auto it = begin; it != end; ++it) {
            sum = sum + *it;  // Add each point in the vector to the sum
        }
        return sum;
    }

    Point<PT, PD> cross(const Point<PT, PD>& other) const {
        Point<PT, PD> result;
        
        for (std::size_t i = 0; i < PD; ++i) {
            result.coordinates[i] = this->coordinates[(i + 1) % PD] * other.coordinates[(i + 2) % PD] - this->coordinates[(i + 2) % PD] * other.coordinates[(i + 1) % PD];
        }

        return result;
    }

    PT norm () const {
        PT sum = 0;
        for (std::size_t i = 0; i < PD; ++i) {
            sum += coordinates[i] * coordinates[i];
        }
        return std::sqrt(sum);
    }

    // Prints the coordinates of the point. If a centroid is set, prints it as well.
    void print() const {
        std::cout << "(";
        for (std::size_t i = 0; i < PD; ++i) {
            std::cout << coordinates[i];
            if (i < PD - 1) std::cout << ", ";
        }
        std::cout << ")";
        if(centroid != nullptr){  // If the centroid exists, print it
            std::cout << "-> Centroid: ";
            centroid->print();
        }

        std::cout << " Point ID: " << id;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point<PT, PD>& point) {
        os << "(";
        for (std::size_t i = 0; i < PD; ++i) {
            os << point.coordinates[i];
            if (i < PD - 1) os << ", ";
        }
        os << ")";
        return os;
    }

    void setID(int id) {
        this->id = id;
    }

    virtual ~Point() = default;
};

#endif
