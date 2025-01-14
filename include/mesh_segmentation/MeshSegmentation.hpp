#ifndef MESH_SEGMENTATION_HPP
#define MESH_SEGMENTATION_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <random>
#include <unordered_map>

#include "geometry/mesh/Mesh.hpp"
#include "geometry/metrics/Metric.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"
#include "clustering/KMeans.hpp"

template <class M>
class MeshSegmentation : public KMeans<double, 3, M>  // Fixed PD to 3 for 3D points
{
public:
    // Constructor: Inherit from KMeans and pass the parameters
    MeshSegmentation(Mesh* mesh, int clusters, double threshold, M metric)
        : KMeans<double, 3, M>(clusters, mesh->getMeshFacesPoints(), threshold, metric)
    {
        this->mesh = mesh;
    }

    // Method to perform mesh segmentation (clustering on a 3D mesh)
    void fit();

    void assignClustersToMesh();

private:
    Mesh* mesh;  // Pointer to the mesh
};

/** Segment the 3D mesh by assigning points to the nearest centroids */
template <class M>
void MeshSegmentation<M>::fit()
{
    // Perform the KMeans clustering from the base class
    KMeans<double, 3, M>::fit();
}

template <class M>
void MeshSegmentation<M>::assignClustersToMesh() {

    std::vector<Point<double, 3>>& points = KMeans<double, 3, M>::getPoints();

    for (auto& point : points) {
        // Get the centroid of the point
        std::shared_ptr<Point<double, 3>> centroid = point.centroid;

        FaceId face = FaceId(point.id);
        int cluster = centroid->id;

        mesh->setFaceCluster(face, cluster);
    }
}


#endif // MESH_SEGMENTATION_HPP