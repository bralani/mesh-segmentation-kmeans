#ifndef GEODESIC_HEAT_METRIC_HPP
#define GEODESIC_HEAT_METRIC_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <cassert>
#include "geometry/mesh/Mesh.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"

#ifdef WIN32
    #include <windows.h>
    #undef max
    #undef min
#endif
#include <igl/heat_geodesics.h>
#ifdef WIN32
    #undef max
    #undef min
#endif

// Forward declaration of the template class
template <typename PT, std::size_t PD>
class GeodesicHeatMetric : public GeodesicMetric<PT, PD>
{
public:
    GeodesicHeatMetric(Mesh &mesh, double percentage_threshold,  std::vector<Point<PT, PD>> data);

protected:
    // Heat geodesics data
    igl::HeatGeodesicsData<PT> data_heat;

    std::vector<PT> computeDistances(const FaceId startFace) const override;
};

#endif // GEODESIC_HEAT_METRIC_HPP