#ifndef GEODESIC_HEAT_METRIC_HPP
#define GEODESIC_HEAT_METRIC_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <cassert>
#include "geometry/mesh/Mesh.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"
#include <igl/heat_geodesics.h>


template <typename PT, std::size_t PD>
class GeodesicHeatMetric : public GeodesicMetric<PT, PD>
{
public:

    GeodesicHeatMetric(Mesh &mesh, double percentage_threshold) : GeodesicMetric<PT, PD>(mesh, percentage_threshold)
    {
        const std::vector<Point<double, 3>> vertices = mesh.getVertices();
        Eigen::MatrixXd V(vertices.size(), 3);
        #pragma omp parallel for
        for (int i = 0; i < vertices.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                V(i, j) = vertices[i].coordinates[j];
            }
        }

        Eigen::MatrixXi F(mesh.numFaces(), 3);
        #pragma omp parallel for
        for (FaceId faceId = 0; faceId < mesh.numFaces(); ++faceId)
        {
            const auto &face = mesh.getFace(faceId);
            for (int i = 0; i < 3; i++)
            {
                F(faceId, i) = face.vertices[i];
            }
        }

        double t = 0.5;
        igl::heat_geodesics_precompute(V, F, t, data);
    }

protected:
    
    // Heat geodesics data
    igl::HeatGeodesicsData<PT> data;

    std::vector<PT> computeDistances(const FaceId startFace) const override
    {
        Eigen::VectorXi gamma(1); gamma << this->mesh->getFace(startFace).vertices[0];
        Eigen::VectorXd dist;
        igl::heat_geodesics_solve(data, gamma, dist);

        std::vector<PT> distFaces;
        distFaces.resize(this->mesh->numFaces());
        #pragma omp parallel for
        for (FaceId faceId = 0; faceId < this->mesh->numFaces(); ++faceId)
        {
            const auto &face = this->mesh->getFace(faceId);

            for (int i = 0; i < 3; i++)
            {
                distFaces[faceId] += dist[face.vertices[i]];
            }
            distFaces[faceId] /= 3;
        }

        return distFaces;
    }

};

#endif // GEODESIC_HEAT_METRIC_HPP