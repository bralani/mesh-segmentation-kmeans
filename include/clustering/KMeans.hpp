#ifndef K_MEANS_HPP
#define K_MEANS_HPP

#include <iostream>
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
#include "clustering/CentroidInitializationMethods/SharedEnum.hpp"

#include "matplotlib-cpp/matplotlibcpp.h"
namespace plt = matplotlibcpp;

#define MIN_NUM_POINTS_CUDA 10000

/** Class implementing the K-Means algorithm using a Kd-Tree data structure
    and employing the filtering method discussed in the Paper */
template <typename PT, std::size_t PD, class M>
class KMeans
{
public:
    /** Constructor
     * clusters: Number of clusters to be generated
     * points: Vector of Points to be used in K-Means
     * dist: Function used as the distance metric between two points
     */
    KMeans(std::size_t clusters, PT treshold, M* metric, 
           int centroidsInitializationMethod, int kInitializationMethod);

    // Destructor: deallocates the tree
    virtual ~KMeans() = default;

    // Fit method to perform the KMeans algorithm
    void fit();

    // Print the results of the algorithm
    void print();

    // Get the data points
    std::vector<Point<PT, PD>>& getPoints();

    // Get the centroids of the clusters
    std::vector<CentroidPoint<PT, PD>>& getCentroids();

    void resetCentroids();
protected:
  M* metric;                                       // Distance metric function
  PT treshold;
  std::size_t numClusters;                                 // Number of clusters
  std::vector<CentroidPoint<PT, PD>> centroids;    // Centroids of clusters

private:

  // Method to extract randomly some initial clusters
  void initializeCentroids(int centroidsInitializationMethod, int kInitializationMethod);
  
};

#endif // K_MEANS_HPP