#ifndef GEODESIC_HEAT_METRIC_HPP
#define GEODESIC_HEAT_METRIC_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <cassert>
#include "geometry/mesh/Mesh.hpp"
#include "geometry/metrics/GeodesicDijkstraMetric.hpp"

#ifdef WIN32
#include <windows.h>
#undef max
#undef min
#endif
#include <igl/heat_geodesics.h>
#include <igl/min_quad_with_fixed.h>
#include <igl/grad.h>
#include <igl/doublearea.h>
#include <igl/cotmatrix.h>
#include <igl/intrinsic_delaunay_cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/PlainVector.h>
#include <igl/massmatrix_intrinsic.h>
#include <igl/grad_intrinsic.h>
#include <igl/boundary_facets.h>
#include <igl/unique.h>
#include <igl/avg_edge_length.h>
#ifdef WIN32
#undef max
#undef min
#endif

typedef Eigen::Matrix<double,Eigen::Dynamic,1> VectorXS;

/** 
 * \class GeodesicHeatMetric
 * \brief A class for computing geodesic distances using heat diffusion-based method.
 *
 * This class inherits from the GeodesicDijkstraMetric class and provides an implementation
 * of geodesic distances based on heat geodesics. It uses data from the igl library
 * to compute these distances across a given mesh.
 *
 * \tparam PT Type of the point (e.g., float, double)
 * \tparam PD Dimension of the point (e.g., 3D)
 */
template <typename PT, std::size_t PD>
class GeodesicHeatMetric : public GeodesicDijkstraMetric<PT, PD>
{
public:
    /**
     * \brief Constructor for the GeodesicHeatMetric class.
     *
     * Initializes the GeodesicHeatMetric with a mesh, a percentage threshold for
     * distance calculations, and additional data related to the heat diffusion.
     *
     * \param mesh The mesh object containing the geometry for the computation.
     * \param percentage_threshold The threshold value to be used in distance calculations.
     * \param data The data points to be used for the heat geodesic computation.
     */
    GeodesicHeatMetric(Mesh &mesh, double percentage_threshold, std::vector<Point<PT, PD>> data);

    /**
     * \brief Computes the geodesic distances from a specified starting face.
     *
     * This method overrides the pure virtual method in the GeodesicDijkstraMetric base class
     * to compute the distances using heat geodesics.
     *
     * \param startFace The face of the mesh from which the geodesic distances will be calculated.
     * \return A vector of computed geodesic distances from the starting face to all other faces.
     */
    std::vector<PT> computeDistances(const FaceId startFace) const override;

protected:
    /**
     * \brief Heat geodesics data.
     *
     * Contains the results and information necessary for performing the heat geodesic
     * calculations using the igl library.
     */
    igl::HeatGeodesicsData<PT> data_heat;
};

#endif // GEODESIC_HEAT_METRIC_HPP