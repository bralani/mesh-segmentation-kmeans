#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <functional> // For passing metric as a function
#include <sstream>    // For std::ostringstream

/**
 * Class to represent a point in an N-dimensional space
 * Attributes:
 * - dimensions: Number of dimensions of the point
 * - values: Coordinate values of the point
 */
class Point {
private:
    int dimensions;              // Number of dimensions (e.g., 2 for 2D, 3 for 3D)
    std::vector<double> values;  // Coordinate values in the N-dimensional space

public:
    /**
     * Constructor
     * @param dimensions Number of dimensions
     * @param values Coordinate values of the point
     * @throws std::invalid_argument if the number of dimensions does not match the size of values
     */
    Point(int dimensions, const std::vector<double>& values) {
        if (values.size() != dimensions) {
            throw std::invalid_argument("Number of dimensions does not match the size of values.");
        }
        this->dimensions = dimensions;
        this->values = values;
    }

    /**
     * Getter for the number of dimensions
     * @return Number of dimensions
     */
    int getDimensions() const {
        return dimensions;
    }

    /**
     * Getter for coordinate values
     * @return A constant reference to the vector of coordinate values
     */
    const std::vector<double>& getValues() const {
        return values;
    }

    /**
     * Setter for coordinate values
     * @param newValues New coordinate values
     * @throws std::invalid_argument if the size of newValues does not match the number of dimensions
     */
    void setValues(const std::vector<double>& newValues) {
        if (newValues.size() != dimensions) {
            throw std::invalid_argument("New values do not match the number of dimensions.");
        }
        values = newValues;
    }

    /**
     * Calculates the distance between this point and another point using a given metric
     * @param other Another point
     * @param metric A function that calculates the distance between two vectors
     * @return The distance between the two points
     * @throws std::invalid_argument if the points have different dimensions
     */
    double distanceTo(const Point& other, const std::function<double(const std::vector<double>&, const std::vector<double>&)>& metric) const {
        if (dimensions != other.getDimensions()) {
            throw std::invalid_argument("Points must have the same number of dimensions to calculate the distance.");
        }
        return metric(values, other.getValues());
    }

    /**
     * Overload of the << operator to print the Point
     * @param os The output stream
     * @param point The Point object to print
     * @return The output stream with the Point's string representation
     */
    friend std::ostream& operator<<(std::ostream& os, const Point& point) {
        os << "Point(" << point.dimensions << "D: [";
        for (size_t i = 0; i < point.values.size(); ++i) {
            os << point.values[i];
            if (i < point.values.size() - 1) {
                os << ", ";
            }
        }
        os << "])";
        return os;
    }
};
