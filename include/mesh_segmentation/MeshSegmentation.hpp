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

/**
 * \class MeshSegmentation
 * \brief Performs segmentation (clustering) on a 3D mesh using K-Means.
 * 
 * This class applies a clustering algorithm to a given 3D mesh, 
 * grouping similar points based on a specified metric.
 * 
 * \tparam M The metric used for measuring distances between points on the mesh (EuclideanMetric, GeodesicMetric, GeodesicHeatMetric).
 */
template <class M>
class MeshSegmentation
{
public:
    /**
     * \brief Constructs a MeshSegmentation object.
     * 
     * Initializes the segmentation process by setting up the metric and 
     * the K-Means clustering algorithm.
     * 
     * \param mesh Pointer to the mesh to be segmented.
     * \param clusters Number of clusters (segments) to create.
     * \param threshold Convergence threshold for the K-Means algorithm.
     * \param num_initialization_method The method used for initializing centroids.
     * \param kInitializationMethod The method used for choosing initial K-Means centers.
     */
    MeshSegmentation(Mesh* mesh, int clusters, double threshold, 
                     int num_initialization_method, int kInitializationMethod)
        : metric(M(*mesh, threshold, mesh->getMeshFacesPoints())),
          kmeans(clusters, threshold, &metric, num_initialization_method, kInitializationMethod),
          mesh(mesh) {}

    /**
     * \brief Performs the mesh segmentation.
     * 
     * Runs the K-Means algorithm to cluster the points of the mesh 
     * based on the given metric.
     */
    void fit();

private:
    Mesh* mesh;  ///< Pointer to the mesh to be segmented.
    M metric;    ///< Metric used to measure distances between points.
    KMeans<double, 3, M> kmeans; ///< K-Means clustering algorithm.
};

/**
 * \brief Runs the segmentation process on the 3D mesh.
 * 
 * Calls the K-Means `fit` method to cluster the mesh points.
 */
template <class M>
void MeshSegmentation<M>::fit()
{
    this->kmeans.fit();
}

#endif // MESH_SEGMENTATION_HPP
