#ifndef POINT_HPP
#define POINT_HPP

#include <array>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <vector>
#include <numeric>

/**
 * \class Point
 * \brief Represents a mathematical point in a multi-dimensional space.
 * 
 * The Point class represents a point in an n-dimensional space, where n is defined
 * by the template parameter PD. The class provides methods for basic vector operations
 * like addition, subtraction, division, cross product (for 3D), and computing the norm.
 * The point also holds an optional centroid (another Point object) and a unique identifier.
 * 
 * \tparam PT The type used for the coordinates (e.g., float, double).
 * \tparam PD The number of dimensions the point has (e.g., 2 for 2D, 3 for 3D).
 */
template <typename PT, std::size_t PD>
class Point {
public:
    /**
     * \brief Array that holds the coordinates of the point.
     * 
     * Each index in the array corresponds to one dimension of the point. The size 
     * of the array is determined by the template parameter PD.
     */
    std::array<PT, PD> coordinates;

    /**
     * \brief Unique identifier for the point.
     * 
     * The ID is typically used to differentiate points in a larger system or mesh.
     * By default, it is set to -1.
     */
    int id;

    /**
     * \brief Optional pointer to the centroid of the point.
     * 
     * This is a shared pointer that may point to another Point object representing 
     * the centroid (if applicable). This is not always set, and defaults to null.
     */
    std::shared_ptr<Point<PT, PD>> centroid = nullptr;

    /**
     * \brief Default constructor for the Point class.
     * 
     * Initializes the coordinates to default values and sets the ID to -1.
     */
    Point();

    /**
     * \brief Constructor for the Point class with specified coordinates.
     * 
     * Initializes the point with the given array of coordinates and an optional ID.
     * 
     * \param coords An array of coordinates for the point.
     * \param id The unique identifier for the point (defaults to -1 if not specified).
     */
    Point(const std::array<PT, PD>& coords, int id = -1);

    /**
     * \brief Constructor that initializes all coordinates with the same value.
     * 
     * This constructor sets each coordinate in the point to the given value.
     * 
     * \param value The value to initialize all coordinates with.
     * \param id The unique identifier for the point (defaults to -1 if not specified).
     */
    Point(PT value, int id = -1);

    /**
     * \brief Returns the number of dimensions of the point.
     * 
     * \return The number of dimensions of the point, which is defined by PD.
     */
    constexpr std::size_t getDimensions() const;

    /**
     * \brief Returns the coordinates of the point.
     * 
     * \return A reference to the array of coordinates.
     */
    const std::array<PT, PD>& getValues() const;

    /**
     * \brief Sets a specific coordinate in the point.
     * 
     * \param value The value to set at the specified index.
     * \param idx The index of the coordinate to set.
     * 
     * \throws std::out_of_range If the provided index is out of bounds.
     */
    void setValue(PT value, int idx);

    /**
     * \brief Sets the centroid of the point.
     * 
     * This method sets the centroid to a specific Point object.
     * 
     * \param centroid A shared pointer to the centroid point.
     */
    void setCentroid(std::shared_ptr<Point<PT, PD>> centroid);

    /**
     * \brief Sets the ID of the point.
     * 
     * \param id The unique identifier to assign to the point.
     */
    void setID(int id);

    /**
     * \brief Equality operator.
     * 
     * Compares two Point objects for equality by comparing their coordinates.
     * 
     * \param other The other Point object to compare against.
     * \return True if the two points have the same coordinates, false otherwise.
     */
    bool operator==(const Point& other) const;

    /**
     * \brief Addition operator.
     * 
     * Adds two Point objects by adding their respective coordinates.
     * 
     * \param other The Point object to add to the current point.
     * \return A new Point object representing the sum of the two points.
     */
    virtual Point<PT, PD> operator+(const Point<PT, PD>& other) const;

    /**
     * \brief Subtraction operator.
     * 
     * Subtracts the coordinates of one Point from another.
     * 
     * \param other The Point object to subtract from the current point.
     * \return A new Point object representing the difference of the two points.
     */
    Point<PT, PD> operator-(const Point<PT, PD>& other) const;

    /**
     * \brief Division operator (point-by-point).
     * 
     * Divides each coordinate of the point by the corresponding coordinate of another point.
     * 
     * \param other The Point object to divide by.
     * \return A new Point object representing the element-wise division.
     */
    Point<PT, PD> operator/(const Point<PT, PD>& other) const;

    /**
     * \brief Division operator (point-by-scalar).
     * 
     * Divides each coordinate of the point by a scalar value.
     * 
     * \param div The scalar value to divide each coordinate by.
     * \return A new Point object representing the point after division by the scalar.
     */
    Point<PT, PD> operator/(double div) const;

    /**
     * \brief Computes the sum of a vector of Point objects.
     * 
     * This static method computes the element-wise sum of a collection of Point objects.
     * 
     * \param begin The iterator pointing to the beginning of the vector.
     * \param end The iterator pointing to the end of the vector.
     * \return A Point object representing the sum of the points in the range.
     */
    static Point<PT, PD> vectorSum(typename std::vector<Point<PT, PD>>::iterator begin,
                                   typename std::vector<Point<PT, PD>>::iterator end);

    /**
     * \brief Cross product.
     * 
     * Computes the cross product of two PT-dimensional points.
     * 
     * \param other The Point object to compute the cross product with.
     * \return A new Point object representing the cross product.
     */
    Point<PT, PD> cross(const Point<PT, PD>& other) const;

    /**
     * \brief Computes the norm (magnitude) of the point.
     * 
     * The norm is computed as the Euclidean distance from the origin (0,0,...,0).
     * 
     * \return The norm (magnitude) of the point.
     */
    PT norm() const;

    /**
     * \brief Prints the coordinates of the point.
     * 
     * This method outputs the coordinates of the point to the console.
     */
    void print() const;

    /**
     * \brief Output stream operator for printing the point.
     * 
     * This allows the point to be printed using the standard output stream.
     * 
     * \param os The output stream to write to.
     * \param point The Point object to output.
     * \return The output stream after writing the point's coordinates.
     */
    friend std::ostream& operator<<(std::ostream& os, const Point<PT, PD>& point) {
        os << "(";
        for (std::size_t i = 0; i < PD; ++i) {
            os << point.coordinates[i];
            if (i < PD - 1) os << ", ";
        }
        os << ")";
        return os;
    }

    /**
     * \brief Sets the centroid of the point.
     * 
     * This method sets the centroid of the point to a specific Point object.
     * 
     * \param point The Point object to set as the centroid.
     */
    void setCentroid(const Point<PT, PD>& point);

    /**
     * \brief Virtual destructor for the Point class.
     * 
     * The destructor is virtual to allow for proper cleanup of derived classes.
     */
    virtual ~Point() = default;
};

#endif
