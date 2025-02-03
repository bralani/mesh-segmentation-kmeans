#ifndef CENTROID_POINT_HPP
#define CENTROID_POINT_HPP

#include <iostream>
#include <array>
#include <cmath>

#include "geometry/point/Point.hpp"
#include "geometry/point/HasWgtCent.hpp"

// The CentroidPoint class inherits from both Point and HasWgtCent
template <typename PT, std::size_t PD>
class CentroidPoint : public Point<PT, PD>, public HasWgtCent<PT, PD>
{
public:
    CentroidPoint();
    CentroidPoint(const Point<PT, PD> &point);

    CentroidPoint<PT, PD> operator+(const HasWgtCent<PT, PD> &other) const;
    void normalize();
    bool operator==(const CentroidPoint<PT, PD> &other) const;
    bool operator==(const Point<PT, PD> &other) const;

    virtual ~CentroidPoint() = default;
};

#endif // CENTROID_POINT_HPP
