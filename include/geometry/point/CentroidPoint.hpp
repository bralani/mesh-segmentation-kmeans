#ifndef CENTROID_POINT_HPP
#define CENTROID_POINT_HPP

#include <iostream>
#include <array>
#include <cmath>

#include "geometry/point/Point.hpp"
#include "geometry/point/HasWgtCent.hpp"

/**
 * \class CentroidPoint
 * \brief Represents a point with a centroid and associated weights.
 * 
 * This class is a specialization of `Point` and `HasWgtCent`. It combines the 
 * functionality of a basic point (from the `Point` class) with the added ability 
 * to store and manipulate weights and centroids (from the `HasWgtCent` class). 
 * It includes operations for normalization, adding weighted centroids, and 
 * comparison with other points.
 * 
 * \tparam PT The data type for the point's coordinates and weights (e.g., float, double).
 * \tparam PD The number of dimensions for the point (size of coordinates array).
 */
template <typename PT, std::size_t PD>
class CentroidPoint : public Point<PT, PD>, public HasWgtCent<PT, PD>
{
public:
    /**
     * \brief Default constructor.
     * 
     * Initializes a `CentroidPoint` object, inheriting the constructor behavior 
     * of both the `Point` and `HasWgtCent` classes.
     */
    CentroidPoint();

    /**
     * \brief Constructor that initializes from an existing Point object.
     * 
     * This constructor initializes the `CentroidPoint` using the given `Point` object, 
     * inheriting the point's coordinates and providing functionality for centroid-related 
     * operations.
     * 
     * \param point The `Point` object used for initialization.
     */
    CentroidPoint(const Point<PT, PD>& point);

    /**
     * \brief Addition operator.
     * 
     * Adds this centroid point with another `HasWgtCent` object, combining their weighted centroids.
     * 
     * \param other The other `HasWgtCent` object to add to this one.
     * \return A new `CentroidPoint` object resulting from the addition.
     */
    CentroidPoint<PT, PD> operator+(const HasWgtCent<PT, PD>& other) const;

    /**
     * \brief Normalizes the centroid point.
     * 
     * Normalizes the point’s coordinates, adjusting them to have a magnitude of 1.
     */
    void normalize();

    /**
     * \brief Equality comparison operator for CentroidPoint objects.
     * 
     * Compares two `CentroidPoint` objects for equality by checking if their coordinates 
     * and weights are the same.
     * 
     * \param other The `CentroidPoint` object to compare with this one.
     * \return True if both objects are equal, otherwise false.
     */
    bool operator==(const CentroidPoint<PT, PD>& other) const;

    /**
     * \brief Equality comparison operator for Point objects.
     * 
     * Compares a `CentroidPoint` with a `Point` object, checking if their coordinates are the same.
     * 
     * \param other The `Point` object to compare with this `CentroidPoint`.
     * \return True if both points are equal, otherwise false.
     */
    bool operator==(const Point<PT, PD>& other) const;

    /**
     * \brief Inequality comparison operator for CentroidPoint objects.
     * 
     * Compares two `CentroidPoint` objects for inequality by checking if their coordinates 
     * or weights are different.
     * 
     * \param other The `CentroidPoint` object to compare with this one.
     * \return True if both objects are not equal, otherwise false.
     */
    bool operator!=(const CentroidPoint<PT, PD>& other) const;

    /**
     * \brief Inequality comparison operator for Point objects.
     * 
     * Compares a `CentroidPoint` with a `Point` object, checking if their coordinates are different.
     * 
     * \param other The `Point` object to compare with this `CentroidPoint`.
     * \return True if both points are not equal, otherwise false.
     */
    bool operator!=(const Point<PT, PD>& other) const;

    /**
     * \brief Virtual default destructor.
     * 
     * Ensures proper destruction of derived objects when the `CentroidPoint` is destroyed, 
     * particularly useful in cases of polymorphism.
     */
    virtual ~CentroidPoint() = default;
};

#endif // CENTROID_POINT_HPP
