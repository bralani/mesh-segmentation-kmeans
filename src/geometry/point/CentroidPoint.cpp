#include "geometry/point/CentroidPoint.hpp"

template <typename PT, std::size_t PD>
CentroidPoint<PT, PD>::CentroidPoint()
    : Point<PT, PD>(), HasWgtCent<PT, PD>() {}

template <typename PT, std::size_t PD>
CentroidPoint<PT, PD>::CentroidPoint(const Point<PT, PD> &point)
    : Point<PT, PD>(point), HasWgtCent<PT, PD>() {}

template <typename PT, std::size_t PD>
CentroidPoint<PT, PD> CentroidPoint<PT, PD>::operator+(const HasWgtCent<PT, PD> &other) const
{
    CentroidPoint<PT, PD> result(*this); // Copy the current object
    for (std::size_t i = 0; i < PD; ++i)
    {
        result.wgtCent[i] = this->wgtCent[i] + other.wgtCent[i]; // Add the weights element-wise
    }
    result.count = this->count + other.count; // Add the count
    return result;
}

template <typename PT, std::size_t PD>
void CentroidPoint<PT, PD>::normalize()
{
    for (std::size_t i = 0; i < PD; ++i)
    {
        this->coordinates[i] = this->wgtCent[i] / this->count; // Set the coordinate as the weighted average
    }
}

template <typename PT, std::size_t PD>
bool CentroidPoint<PT, PD>::operator==(const CentroidPoint<PT, PD> &other) const
{
    // Compare the integer part of the coordinates
    for (std::size_t i = 0; i < PD; ++i)
    {
        if (std::floor(this->coordinates[i]) != std::floor(other.coordinates[i]))
        {
            return false; // Coordinates differ in their integer part
        }
    }

    return true; // If coordinates are the same (integer part), return true
}

// Explicit template instantiation
template class CentroidPoint<double, 3>;
template class CentroidPoint<double, 2>;