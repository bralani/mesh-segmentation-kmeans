#ifndef EUCLIDEANMETRIC_HPP
#define EUCLIDEANMETRIC_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>

#include "geometry/kdtree/KDTree.hpp"
#include "geometry/metrics/Metric.hpp"
#include "geometry/mesh/Mesh.hpp"

#ifdef USE_CUDA
// Declaration of the CUDA kernel function (defined in kmeans.cu)
void kmeans_cuda(int K, int dim, int numPoints, float *points, float *centroids, int *cluster_assignment, float threshold);
#endif

#define MIN_NUM_POINTS_CUDA 10000
#define MAX_ITERATIONS 100

/**
 * \class EuclideanMetric
 * \brief A class that calculates Euclidean distance for clustering and metric purposes.
 * 
 * This class provides functionality to compute Euclidean distances between points 
 * and can operate both on CPU and GPU. On the CPU, it uses a KDTree structure (with OpenMP) to
 * efficiently find nearest neighbors and compute distances. On the GPU, it uses CUDA
 * to parallelize the computation and speed up the process.
 * 
 * \tparam PT Type of the point (e.g., float, double)
 * \tparam PD Dimension of the point (e.g., 2D, 3D)
 */
template <typename PT, std::size_t PD>
class EuclideanMetric : public Metric<PT, PD>
{
public:
    /**
     * \brief Default constructor for EuclideanMetric.
     * 
     * Initializes an instance of the EuclideanMetric class with default settings.
     */
    EuclideanMetric() = default;

    /**
     * \brief Constructor that initializes the metric with data points and a threshold.
     * 
     * \param data The data points used for the metric computation.
     * \param threshold The threshold value for metric computation.
     */
    EuclideanMetric(std::vector<Point<PT, PD>> data, double threshold);

    /**
     * \brief Constructor that initializes the metric with a mesh, threshold, and data points.
     * 
     * \param mesh The mesh containing the geometry for the metric computation.
     * \param percentage_threshold The threshold percentage used for calculations.
     * \param data The data points used for the metric computation.
     */
    EuclideanMetric(Mesh &mesh, double percentage_threshold, std::vector<Point<PT, PD>> data);

    /**
     * \brief Computes the Euclidean distance between two points.
     * 
     * This method calculates the straight-line Euclidean distance between two points.
     * 
     * \param a The first point.
     * \param b The second point.
     * \return The Euclidean distance between the two points.
     */
    static PT distanceTo(const Point<PT, PD> &a, const Point<PT, PD> &b);

    /**
     * \brief Setup method to initialize necessary components before fitting the model.
     * 
     * This method prepares the metric by setting up any necessary data structures.
     */
    void setup() override;

    /**
     * \brief Fits the model using CPU-based computation.
     * 
     * This method performs the fitting operation using the CPU, updating centroids 
     * and other model parameters.
     */
    void fit_cpu() override;

#ifdef USE_CUDA
    /**
     * \brief Fits the model using GPU-based computation.
     * 
     * This method performs the fitting operation using CUDA, utilizing GPU resources 
     * to speed up the calculations.
     */
    void fit_gpu() override;
#endif

    /**
     * \brief Returns the data points used for clustering.
     * 
     * \return A reference to the vector of data points.
     */
    std::vector<Point<PT, PD>> &getPoints() override;

private:
    Mesh *mesh; /**< Pointer to the mesh object for the metric calculation. */
    double treshold; /**< The threshold value for the metric. */
    std::unique_ptr<KdTree<PT, PD>> kdtree; /**< Pointer to the KDTree used for nearest-neighbor search. */

    /**
     * \brief Filters the data points based on certain criteria.
     * 
     * This private method is used to filter out irrelevant or unneeded data points 
     * from the dataset.
     */
    void filter();

    /**
     * \brief Recursively filters data points in the KDTree structure.
     * 
     * \param node The current KDNode being processed.
     * \param candidates A list of candidate centroids to compare.
     * \param depth The current depth of the recursion.
     */
    void filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, int depth);

    /**
     * \brief Finds the closest candidate centroid to a given target point.
     * 
     * \param candidates A list of candidate centroids.
     * \param target The target point to find the closest centroid to.
     * \return The closest centroid to the target.
     */
    std::shared_ptr<CentroidPoint<PT, PD>> findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target);

    /**
     * \brief Checks if a point is farther from a reference point than another.
     * 
     * \param z The first point to compare.
     * \param zStar The second reference point.
     * \param node The current KDNode being processed.
     * \return True if point z is farther than zStar, false otherwise.
     */
    bool isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node);

    /**
     * \brief Assigns a centroid to a node in the KDTree.
     * 
     * \param node The current KDNode.
     * \param centroid The centroid to be assigned.
     */
    void assignCentroid(std::unique_ptr<KdNode<PT, PD>> &node, const std::shared_ptr<CentroidPoint<PT, PD>> &centroid);

    /**
     * \brief Checks for convergence of the clustering algorithm.
     * 
     * This method checks if the clustering algorithm has converged, 
     * i.e., if centroids are no longer changing significantly.
     * 
     * \param iter The current iteration number.
     * \return True if the algorithm has converged, false otherwise.
     */
    bool checkConvergence(int iter);

    /**
     * \brief Stores the centroids after the fitting process.
     * 
     * This method stores the final centroids after the fitting process has completed.
     */
    void storeCentroids() override;

    /**
     * \brief Updates the face clusters based on the clustering results.
     * 
     * This method updates the mesh faces with the corresponding cluster assignments.
     */
    void updateFaceClusters();
};

#endif
