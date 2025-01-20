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
#include "geometry/metrics/GeodesicMetric.hpp"
#include "clustering/KMeans.hpp"


typedef GeodesicMetric<double, 3> MetricMeshSegmentation;

class MeshSegmentation
{
public:
    MeshSegmentation(Mesh* mesh, int clusters, double threshold, int num_initialization_method)
    : kmeans(clusters, mesh->getMeshFacesPoints(), threshold, MetricMeshSegmentation(*mesh, threshold), num_initialization_method),
      mesh(mesh) {}

    // Method to perform mesh segmentation (clustering on a 3D mesh)
    void fit();

private:
    Mesh* mesh;  // Pointer to the mesh
    KMeans<double, 3, MetricMeshSegmentation> kmeans; // KMeans object
};

/** Segment the 3D mesh by assigning points to the nearest centroids */
void MeshSegmentation::fit()
{
    this->kmeans.fit();
}


#endif // MESH_SEGMENTATION_HPP