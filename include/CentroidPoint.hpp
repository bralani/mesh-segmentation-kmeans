#ifndef CENTROID_POINT_HPP
#define CENTROID_POINT_HPP

#include "Point.hpp"
#include "HasWgtCent.hpp"
#include <iostream>
#include <array>

template <typename PT, std::size_t PD>
class CentroidPoint : public Point<PT, PD>, public HasWgtCent<PT, PD> {
public:
    // Default constructor
    CentroidPoint() 
        : Point<PT, PD>(), HasWgtCent<PT, PD>() {}

    // Constructor with a Point as input
    CentroidPoint(const Point<PT, PD>& point)
        : Point<PT, PD>(point), HasWgtCent<PT, PD>() {}

    CentroidPoint<PT, PD> operator+(const HasWgtCent<PT, PD>& other) const {
        CentroidPoint<PT, PD> result(*this);
        for (std::size_t i = 0; i < PD; ++i) {
            result.wgtCent[i] = this->wgtCent[i] + other.wgtCent[i];
        }
        result.count = this->count + other.count;
        return result;
    }


    void normalize() {
        for (std::size_t i = 0; i < PD; ++i) {
            this->coordinates[i] = this->wgtCent[i] / this->count;
        }
    }


};

#endif // CENTROID_POINT_HPP
