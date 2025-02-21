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
 * \brief Implements the K-Means clustering algorithm.
 *
 * This class performs clustering on a dataset using the K-Means algorithm, supporting various centroid initialization methods.
 *
 * \tparam PT Type of the data points (e.g., float, double, etc.).
 * \tparam PD Dimension of the data points.
 * \tparam M Distance metric type.
 */
template <typename PT, std::size_t PD, class M>
class KMeans
{
public:
    /**
     * \brief Constructor for KMeans.
     * \param clusters Number of clusters.
     * \param treshold Convergence threshold.
     * \param metric Distance metric function.
     * \param centroidsInitializationMethod Method for initializing centroids.
     * \param kInitializationMethod Method for determining the optimal number of clusters.
     */
    KMeans(std::size_t clusters, PT treshold, M* metric, 
           int centroidsInitializationMethod, int kInitializationMethod);

    /**
     * \brief Destructor for KMeans.
     */
    virtual ~KMeans() = default;    
    
    /**
     * \brief Runs the K-Means clustering algorithm.
     */
    void fit();

    /** 
     * \brief Prints the results of the K-Means algorithm.
     * 
     * This method outputs the final centroids and the points assigned to each cluster.
     */
    void print();

    /**
     * \brief Retrieves the dataset points.
     * \return Reference to the vector of points.
     */
    std::vector<Point<PT, PD>>& getPoints();

    /**
     * \brief Retrieves the centroids of the clusters.
     * \return Reference to the vector of centroid points.
     */
    std::vector<CentroidPoint<PT, PD>>& getCentroids();

    /**
     * \brief Resets the centroids.
     */
    void resetCentroids();

    /**
     * \brief Sets the number of clusters.
     * \param numC New number of clusters.
     */
    void setNumClusters(std::size_t numC);

protected:
    M* metric;                                       ///< Distance metric function used in clustering.
    PT treshold;                                     ///< Threshold value for convergence.
    std::size_t numClusters;                         ///< Number of clusters to generate.
    std::vector<CentroidPoint<PT, PD>> centroids;    ///< Centroids of clusters.

private:
    /**
     * \brief Initializes the centroids using a selected method.
     * \param centroidsInitializationMethod Method for initializing centroids.
     * \param kInitializationMethod Method for determining the optimal number of clusters.
     */
    void initializeCentroids(int centroidsInitializationMethod, int kInitializationMethod);
};

#endif // K_MEANS_HPP
