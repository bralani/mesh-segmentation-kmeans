#ifndef K_MEANS_HPP
#define K_MEANS_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <random>
#include <omp.h>

#include "geometry/point/Point.hpp"
#include "geometry/point/CentroidPoint.hpp"

#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"
#include "clustering/CentroidInitializationMethods/KDECentroidMatrix.hpp"
#include "clustering/CentroidInitializationMethods/KDECentroid.hpp"
#include "clustering/CentroidInitializationMethods/RandomCentroids.hpp"
#include "clustering/CentroidInitializationMethods/MostDistantCentroids.hpp"
#include "clustering/CentroidInitializationMethods/kInitMethods.hpp"
#include "clustering/CentroidInitializationMethods/Elbowmethod.hpp"
#include "clustering/CentroidInitializationMethods/KDEKInitMehod.hpp"
#include "clustering/CentroidInitializationMethods/Silhouette.hpp"
#include "clustering/CentroidInitializationMethods/SharedEnum.hpp"

#define MIN_NUM_POINTS_CUDA 10000

/** 
 * \class KMeans
 * \brief Class implementing the K-Means clustering algorithm.
 * 
 * The class performs the K-Means algorithm by clustering a set of data points into a specified number of clusters. 
 * It supports various centroid initialization methods and utilizes a distance metric for clustering.
 * 
 * \tparam PT The type of the point coordinates (e.g., double, int).
 * \tparam PD The number of dimensions of the points (e.g., 2 for 2D, 3 for 3D).
 * \tparam M The type of the distance metric (e.g., EuclideanMetric, GeodesicMetric, GeodesicHeatMetric).
 */
template <typename PT, std::size_t PD, class M>
class KMeans
{
public:
    /** 
     * \brief Constructor to initialize the KMeans algorithm with necessary parameters.
     * 
     * This constructor takes the number of clusters, a threshold value for convergence, 
     * the metric function to compute distances between points, and the methods for 
     * centroid initialization.
     * 
     * \param clusters The number of clusters to generate.
     * \param treshold The threshold value used to determine when the algorithm has converged.
     * \param metric A pointer to the metric function used to calculate distances between points.
     * \param centroidsInitializationMethod The method to initialize centroids.
     * \param kInitializationMethod Additional initialization method for centroids.
     */
    KMeans(std::size_t clusters, PT treshold, M* metric, 
           int centroidsInitializationMethod, int kInitializationMethod);

    /** 
     * \brief Destructor for cleaning up resources.
     * 
     * The destructor handles any necessary cleanup and deallocation when the 
     * KMeans object is destroyed.
     */
    virtual ~KMeans() = default;

    /** 
     * \brief Fit method to perform the K-Means algorithm.
     * 
     * This method performs the K-Means clustering process: 
     * 1. Initializes centroids, 
     * 2. Assigns points to clusters based on the closest centroid, 
     * 3. Recomputes centroids and repeats until convergence.
     */
    void fit();

    /** 
     * \brief Prints the results of the K-Means algorithm.
     * 
     * This method outputs the final centroids and the points assigned to each cluster.
     */
    void print();

    /** 
     * \brief Getter for the data points used in clustering.
     * 
     * \return A reference to the vector of points used for clustering.
     */
    std::vector<Point<PT, PD>>& getPoints();

    /** 
     * \brief Getter for the centroids of the clusters.
     * 
     * \return A reference to the vector of centroids.
     */
    std::vector<CentroidPoint<PT, PD>>& getCentroids();

    /** 
     * \brief Resets the centroids.
     * 
     * This method clears and reinitializes the centroids for the next run or adjustment of the algorithm.
     */
    void resetCentroids();

    /** 
     * \brief Setter for the number of clusters.
     * 
     * This method allows updating the number of clusters if required before running the algorithm.
     * 
     * \param numC The number of clusters.
     */
    void setNumClusters(std::size_t numC);

protected:
  M* metric;                                       ///< Distance metric function used in clustering.
  PT treshold;                                     ///< Threshold value for convergence.
  std::size_t numClusters;                         ///< Number of clusters to generate.
  std::vector<CentroidPoint<PT, PD>> centroids;    ///< Centroids of clusters.

private:
  /** 
   * \brief Method to initialize the centroids using the specified initialization method.
   * 
   * This method selects the centroids based on the chosen initialization strategy 
   * such as random, most distant, or based on a density function.
   * 
   * \param centroidsInitializationMethod The method to initialize centroids.
   * \param kInitializationMethod An additional method to adjust initialization.
   */
  void initializeCentroids(int centroidsInitializationMethod, int kInitializationMethod);
};

#endif // K_MEANS_HPP