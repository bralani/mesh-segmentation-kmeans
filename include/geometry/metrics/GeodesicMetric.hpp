#ifndef GEODESICMETRIC_HPP
#define GEODESICMETRIC_HPP

#include <queue>
#include <utility>
#include <optional>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <omp.h>
#include "geometry/mesh/Mesh.hpp"
#include "geometry/metrics/Metric.hpp"
#include "geometry/point/CentroidPoint.hpp"

#ifdef USE_CUDA
// CUDA kernel for geodesic-based clustering
void kmeans_cuda_geodesic(
    int N,
    int K,
    int dim,
    std::vector<float>& h_faceBaricenter, 
    std::vector<float>& h_centroids,
    std::vector<int>&   h_faceCluster,
    const std::vector<std::vector<int>>& adjacency,
    float threshold
);
#endif

/**
 * \class GeodesicMetric
 * \brief A class for computing geodesic distances on a mesh with Dijkstra's algorithm
 * 
 * This class provides an implementation for computing geodesic distances between faces
 * on a mesh. The distances are computed using various methods, including a Dijkstra-like 
 * algorithm in the `computeDistances` function. Additionally, it supports centroid-based 
 * clustering on meshes with geodesic distance metrics.
 * 
 * \tparam PT Type of the point (e.g., float, double)
 * \tparam PD Dimension of the point (e.g., 3D)
 */
template <typename PT, std::size_t PD>
class GeodesicMetric : public Metric<PT, PD>
{
public:
    /**
     * \brief Constructor that initializes the geodesic metric with a mesh and data points.
     * 
     * \param mesh The mesh containing the geometry to calculate geodesics over.
     * \param percentage_threshold The threshold value for geodesic distance calculations.
     * \param data A collection of points (faces) to work with in the metric calculation.
     */
    GeodesicMetric(Mesh &mesh, double percentage_threshold, std::vector<Point<PT, PD>> data);

    /**
     * \brief Prepares the metric by setting up necessary data structures and initializations.
     * 
     * This method is used to perform any necessary setup before fitting or calculating geodesics.
     */
    void setup() override;

    /**
     * \brief Fits the geodesic model using CPU-based computation.
     * 
     * This method computes the geodesic distances and centroid assignments using the CPU.
     */
    void fit_cpu() override;

#ifdef USE_CUDA
    /**
     * \brief Fits the geodesic model using GPU-based computation.
     * 
     * This method computes the geodesic distances and centroid assignments using the GPU
     * for faster computation, utilizing CUDA kernels.
     */
    void fit_gpu() override;
#endif

    /**
     * \brief Finds the closest face on the mesh to a given point (centroid).
     * 
     * This method finds the face in the mesh that is closest 
     * to the provided point, based on Euclidean distance.
     * 
     * \param centroid The point for which the closest face is to be found.
     * \return The FaceId of the closest face to the centroid.
     */
    FaceId findClosestFace(const Point<PT, PD> &centroid) const;

    /**
     * \brief Gets the data points (faces) used for the geodesic calculations.
     * 
     * \return A reference to the vector of points (faces).
     */
    std::vector<Point<PT, PD>>& getPoints() override;

protected:
    Mesh *mesh; /**< Pointer to the mesh used in geodesic calculations. */
    std::unordered_map<FaceId, std::vector<PT>> distances; /**< Stores computed geodesic distances for each face. */
    int oldPoints = 0; /**< Keeps track of the number of points from previous iterations. */
    double avgDistances; /**< Stores the average geodesic distance used for convergence checks. */

    /**
     * \brief Computes the Euclidean distance between two points.
     * 
     * This helper method computes the straight-line Euclidean distance between two points 
     * in the space.
     * 
     * \param a The first point.
     * \param b The second point.
     * \return The Euclidean distance between the two points.
     */
    double computeEuclideanDistance(const Point<PT, PD>& a, const Point<PT, PD>& b) const;

    /**
     * \brief Computes the geodesic distances starting from a given face using Dijkstra's algorithm.
     * 
     * This method uses Dijkstra's algorithm to compute the shortest geodesic distances from 
     * a given starting face to all other faces in the mesh. The algorithm explores the 
     * neighboring faces and accumulates the geodesic distances.
     * 
     * \param startFace The starting face from which distances will be calculated.
     * \return A vector of computed geodesic distances for the mesh faces.
     */
    virtual std::vector<PT> computeDistances(const FaceId startFace) const;

    /**
     * \brief Stores the centroids after the fitting process.
     * 
     * This method stores the calculated centroids for use in future iterations or analysis.
     */
    void storeCentroids() override;

    /**
     * \brief Computes the dihedral angle between two mesh faces.
     * 
     * This method computes the angle between two adjacent faces, based on their normal vectors.
     * 
     * \param f1 The first face.
     * \param f2 The second face.
     * \return The dihedral angle between the two faces in radians.
     */
    double dihedralAngle(const Face& f1, const Face& f2) const;

    /**
     * \brief Sets up the average distance between neighbors faces.
     * 
     * This method computes the average distance value between neighbors faces.
     * 
     * \return The computed average distance.
     */
    double setupAvg();

    /**
     * \brief Checks for convergence based on the number of iterations and distance changes.
     * 
     * This method checks if the geodesic model has converged by comparing the current 
     * geodesic distances to those from previous iterations.
     * 
     * \param iter The current iteration number.
     * \return True if convergence is reached, false otherwise.
     */
    bool checkConvergence(int iter);
};

#endif // GEODESICMETRIC_HPP
