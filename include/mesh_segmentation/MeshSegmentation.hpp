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
#include "geometry/metrics/GeodesicHeatMetric.hpp"
#include "clustering/KMeans.hpp"

template <class M>
class MeshSegmentation
{
public:
    MeshSegmentation(Mesh* mesh, int clusters, double threshold, int num_initialization_method, int kInitializationMethod)
    : metric(M(*mesh, threshold)),
      kmeans(clusters, mesh->getMeshFacesPoints(), threshold, &metric, num_initialization_method, kInitializationMethod),
      mesh(mesh) {}

    // Method to perform mesh segmentation (clustering on a 3D mesh)
    void fit();

private:
    Mesh* mesh;  // Pointer to the mesh
    M metric;                   // Metric object 
    KMeans<double, 3, M> kmeans; // KMeans object
};

/** Segment the 3D mesh by assigning points to the nearest centroids */
template <class M>
void MeshSegmentation<M>::fit()
{
    this->kmeans.fit();
}


#endif // MESH_SEGMENTATION_HPP