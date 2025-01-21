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
#include "geometry/metrics/Metric.hpp"
#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"

#include "clustering/CentroidInitializationMethods/KDECentroidMatrix.hpp"
#include "clustering/CentroidInitializationMethods/KDECentroid.hpp"
#include "clustering/CentroidInitializationMethods/RandomCentroids.hpp"
#include "clustering/CentroidInitializationMethods/MostDistantCentroids.hpp"


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
  KMeans(int clusters, std::vector<Point<PT, PD>> points, PT treshold, M metric, int centroidsInitializationMethod)
      : numClusters(clusters), data(points), treshold(treshold), metric(metric)
  {
    initializeCentroids(centroidsInitializationMethod);
  }

  // Destructor: deallocates the tree
  virtual ~KMeans() = default;

  // Fit method to perform the KMeans algorithm
  void fit();

  void print();

  std::vector<Point<PT, PD>>& getPoints() { return data; }

protected:
  M metric;                                        // Distance metric function
  std::vector<Point<PT, PD>> data;                 // Data points
  std::vector<CentroidPoint<PT, PD>> centroids;    // Centroids of clusters
  PT treshold;
  int numClusters;                                 // Number of clusters

private:

  // Method to extract randomly some initial clusters
  void initializeCentroids(int centroidsInitializationMethod);
  
};

/** Extracts randomly "numClusters" initial Centroids from the same data that were provided
 */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::initializeCentroids(int centroidsInitializationMethod)
{
  if (centroidsInitializationMethod < 0 || centroidsInitializationMethod > 3) {
      throw std::invalid_argument("Not a valid centroids initialization method!");
  }

  CentroidInitMethod<double, PD>* cim; 

  if(centroidsInitializationMethod == 0)
    cim = new RandomCentroidInit(data, numClusters);
  else if(centroidsInitializationMethod == 1)
    cim = new KDE(data, numClusters);
  else if(centroidsInitializationMethod == 2)
    cim = new MostDistanceClass(data, numClusters);
  else 
    cim = new KDE3D(data, numClusters);
  
  cim->findCentroid(this->centroids);
  std::cout << "Centroids: \n";
  for (auto &p : centroids)
  {
    p.print();
    std::cout << "\n";
  }

}

/** Fits the KMeans algorithm to the data */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::fit()
{
  metric.setCentroids(centroids);

  #ifdef USE_CUDA
    if (data.size() > MIN_NUM_POINTS_CUDA) {
      metric.fit_gpu();
    } else {
      metric.fit_cpu();
    }
  #else
    metric.fit_cpu();
  #endif
}

/** DEBUG FUNCTION */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::print()
{
  std::cout << "-----------------------" << std::endl;
  std::cout << "Centroids: \n";
  for (auto &p : centroids)
  {
    p.print();
    std::cout << "\n";
  }

  std::cout << "-----------------------" << std::endl;
  std::cout << "Points: \n";

  // Color map based on the centroid index
  std::vector<std::string> colors = {"r", "g", "b", "y", "m", "c", "k", "orange", "purple", "brown"};

  // Create vectors for points associated with each centroid
  std::vector<std::vector<double>> x_points_per_centroid(centroids.size());
  std::vector<std::vector<double>> y_points_per_centroid(centroids.size());

  for (auto &p : data)
  {
    p.print();
    if (p.centroid != nullptr) // Check if a centroid is set
    {
      std::cout << " -> Centroid: ";
      p.centroid->print();

      // Find the index of the corresponding centroid
      // Assuming Point<PT, PD> is compatible with CentroidPoint<PT, PD>
      auto centroid_candidate = CentroidPoint<PT, PD>(*p.centroid); // Convert Point to CentroidPoint
      auto it = std::find(centroids.begin(), centroids.end(), centroid_candidate);

      if (it != centroids.end())
      {
        int centroid_index = std::distance(centroids.begin(), it);

        // Add the point's coordinates to the appropriate centroid's point vector
        x_points_per_centroid[centroid_index].push_back(p.coordinates[0]);
        y_points_per_centroid[centroid_index].push_back(p.coordinates[1]);
      }
      else
      {
        std::cerr << "Error: Centroid not found!" << std::endl;
      }
    }
    else
    {
      std::cout << " -> No centroid assigned.";
    }
    std::cout << "\n";
  }

  try
  {
    // Plot the points for each centroid with its associated color
    for (size_t i = 0; i < centroids.size(); ++i)
    {
      // Plot the points for the current centroid with its corresponding color
      plt::scatter(x_points_per_centroid[i], y_points_per_centroid[i], 20, {{"color", colors[i % colors.size()]}, {"label", "Centroid " + std::to_string(i)}});
    }

    // Plot the centroids with bigger markers (e.g., size 50) and a distinct color (e.g., black)
    std::vector<double> x_centroids, y_centroids;
    for (const auto &centroid : centroids)
    {
      x_centroids.push_back(centroid.coordinates[0]);
      y_centroids.push_back(centroid.coordinates[1]);
    }
    plt::scatter(x_centroids, y_centroids, 50, {{"color", "k"}, {"label", "Centroids"}}); // Black color for centroids

    // Set title and labels
    plt::title("Plot from CSV Data");
    plt::xlabel("X-axis");
    plt::ylabel("Y-axis");

    // Save the plot as an image
    plt::save("/app/output/plot_centroid.png");

    plt::show();

    return;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return;
  }
}

#endif // K_MEANS_HPP