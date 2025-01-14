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
#include "clustering/KDTree.hpp"
#include "geometry/metrics/Metric.hpp"

#include "matplotlib-cpp/matplotlibcpp.h"
namespace plt = matplotlibcpp;

#define MIN_NUM_POINTS_CUDA 10000
#define MAX_ITERATIONS 100

#ifdef USE_CUDA
  // Declaration of the CUDA kernel function (defined in kmeans.cu)
  void kmeans_cuda(int K, int dim, int numPoints, float *points, float *centroids, int *cluster_assignment, float threshold);
#endif

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
  KMeans(int clusters, std::vector<Point<PT, PD>> points, PT treshold, M metric)
      : numClusters(clusters), data(points), treshold(treshold), metric(metric)
  {
    initializeCentroids();

    #ifdef USE_CUDA
      if (data.size() > MIN_NUM_POINTS_CUDA) {
        // In cuda mode, the kdtree is not used
        kdtree = nullptr;
      } else {
        kdtree = new KdTree<PT, PD>(data);
      }
    #else
      kdtree = new KdTree<PT, PD>(data);
    #endif
  }

  // Destructor: deallocates the tree
  virtual ~KMeans() = default;

  // Fit method to perform the KMeans algorithm
  void fit();

  void print();

  std::vector<Point<PT, PD>>& getPoints() { return data; }

protected:
  M metric;                                        // Distance metric function

private:
  int numClusters;                                 // Number of clusters
  std::vector<Point<PT, PD>> data;                 // Data points
  std::vector<CentroidPoint<PT, PD>> centroids;    // Centroids of clusters
  std::vector<CentroidPoint<PT, PD>> oldCentroids; // Centroids on the previous filter
  KdTree<PT, PD> *kdtree;                          // Kd-Tree structure
  PT treshold;

  // Method to extract randomly some initial clusters
  void initializeCentroids();

  // Filters the given centers based on the KD-tree structure
  void filter();

  // Recursive helper for the filter function
  void filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, int depth = 0);

  // Determines if a point `z` is farther from a bounding box than `zStar`
  bool isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node);

  // Finds the closest candidate (centroid) to a given point
  std::shared_ptr<CentroidPoint<PT, PD>> findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target);

  void assignCentroid(std::unique_ptr<KdNode<PT, PD>> &node, const std::shared_ptr<CentroidPoint<PT, PD>> &centroid);

  bool checkConvergence(int iter);
  
  // Fit the KMeans algorithm on the CPU or GPU
  void fit_cpu();

  #ifdef USE_CUDA
    void fit_gpu();
  #endif
};

/** Extracts randomly "numClusters" initial Centroids from the same data that were provided
 */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::initializeCentroids()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  // Contenitore per memorizzare gli indici casuali
  std::vector<int> indices(data.size());
  std::iota(indices.begin(), indices.end(), 0); // Riempie gli indici da 0 a data.size() - 1

  // Selezione casuale senza ripetizioni
  std::vector<int> sampledIndices;
  std::sample(indices.begin(), indices.end(), std::back_inserter(sampledIndices), numClusters, gen);

  int i = 0;
  // Creazione dei centroidi iniziali
  for (int idx : sampledIndices)
  {
    centroids.push_back(CentroidPoint<PT, PD>(data[idx]));
    centroids[i].setID(i);
    i++;
  }
}

/** Fits the KMeans algorithm to the data */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::fit()
{
  #ifdef USE_CUDA
    if (data.size() > MIN_NUM_POINTS_CUDA) {
      fit_gpu();
    } else {
      fit_cpu();
    }
  #else
    fit_cpu();
  #endif
}


/** Kmeans on CPU */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::fit_cpu() {
  metric.setCentroids(centroids);
  metric.initialSetup();

  bool convergence = false;
  int iter = 0;
  while (!convergence)
  { 
    this->filter();
    convergence = checkConvergence(iter);
    oldCentroids = centroids;
    metric.setup();
    iter++;
  }
}

#ifdef USE_CUDA
/** Kmeans on GPU */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::fit_gpu() {
  
  // Convert the data to a flat array
  float *data_flat = new float[data.size() * PD];
  #pragma omp parallel for collapse(2)
  for (int i = 0; i < data.size(); i++)
  {
    for (int j = 0; j < PD; j++)
    {
      data_flat[i * PD + j] = data[i].coordinates[j];
    }
  }

  float *centroids_flat = new float[numClusters * PD];
  #pragma omp parallel for collapse(2)
  for (int i = 0; i < numClusters; i++)
  {
    for (int j = 0; j < PD; j++)
    {
      centroids_flat[i * PD + j] = centroids[i].coordinates[j];
    }
  }

  int *cluster_assignment = new int[data.size()];

  // Call the CUDA kernel function
  kmeans_cuda(numClusters, PD, data.size(), data_flat, centroids_flat, cluster_assignment, (float)treshold);

  // Convert the centroids back to the original format
  #pragma omp parallel for collapse(2)
  for (int i = 0; i < numClusters; i++)
  {
    for (int j = 0; j < PD; j++)
    {
      centroids[i].coordinates[j] = centroids_flat[i * PD + j];
    }
  }

  // Convert the cluster assignments back to the original format
  #pragma omp parallel for
  for (int i = 0; i < data.size(); i++) 
  {
    std::shared_ptr<CentroidPoint<PT, PD>> centroid_ptr = std::make_shared<CentroidPoint<PT, PD>>(centroids[cluster_assignment[i]]);
    data[i].setCentroid(centroid_ptr);
  }
}
#endif

template <typename PT, std::size_t PD, class M>
bool KMeans<PT, PD, M>::checkConvergence(int iter)
{
  if (iter > MAX_ITERATIONS)
    return true;

  if (oldCentroids.empty())
    return false;
  else
  {
    PT dist = 0;
    for (int i = 0; i < centroids.size(); i++)
    {
      dist += metric.distanceTo(centroids[i], oldCentroids[i]);
    }
    dist = dist / centroids.size();
    if (dist > treshold)
      return false;
    return true;
  }
}

/** Implements the "filter" function discussed in the paper*/
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::filter()
{

  // Convert centers to shared pointers. During filterRecursive their values will be modified
  std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> centersPointers;
  for (CentroidPoint<PT, PD> &z : centroids)
  {
    z.resetCount();
    centersPointers.push_back(std::shared_ptr<CentroidPoint<PT, PD>>(&z, [](CentroidPoint<PT, PD> *) {}));
  }

  // Start the recursive filtering process
  #pragma omp parallel 
  {
    #pragma omp single
    filterRecursive(kdtree->getRoot(), centersPointers, 0);
  }

  /** We need to divide the wgtCent of the centroid for the number of points which has it as centroid */
  for (CentroidPoint<PT, PD> &c : centroids)
  {
    c.normalize();
  }
}

/**  Recursive function for filtering. Follow exactly the paper */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, int depth)
{
  if (!node)
    return;

  if (!node->left && !node->right)
  {
    // Leaf node: find the closest candidate and update it
    std::shared_ptr<CentroidPoint<PT, PD>> zStar_ptr = findClosestCandidate(candidates, node->wgtCent);
    *zStar_ptr = *zStar_ptr + *node;

    // The leaf nodes are directly linked to the data we provided. Here we assign each Point its centroid
    node->myPoint->setCentroid(zStar_ptr);
    return;
  }
  else
  {
    // Internal node: compute the midpoint of the cell
    Point<PT, PD> cellMidpoint;
    for (std::size_t i = 0; i < PD; ++i)
    {
      cellMidpoint.setValue((node->cellMin[i] + node->cellMax[i]) / PT(2), i);
    }

    // Find the closest candidate to the cell midpoint
    std::shared_ptr<CentroidPoint<PT, PD>> zStar_ptr = findClosestCandidate(candidates, cellMidpoint);

    // Filter candidates based on proximity to zStar
    std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> filteredCandidates;
    filteredCandidates.reserve(candidates.size());
    for (auto &z : candidates)
    {
      if (z == zStar_ptr)
      {
        filteredCandidates.push_back(z);
      }
      else if (!isFarther(*z, *zStar_ptr, *node))
      {
        filteredCandidates.push_back(z);
      }
    }

    int max_threads = omp_get_max_threads();
    bool can_parallelize = (depth < std::log2(max_threads)+1);

    if (filteredCandidates.size() == 1)
    {
      // If an internal node has a single candidate, just update it and spread the candidate among the subtree
      *filteredCandidates[0] = *filteredCandidates[0] + *node;

      if (can_parallelize)
      {
        #pragma omp parallel
        {
          #pragma omp single
          {
            // Create tasks to assign centroids to the left and right subtrees
            #pragma omp task shared(node, filteredCandidates)
            assignCentroid(node->left, filteredCandidates[0]);

            #pragma omp task shared(node, filteredCandidates)
            assignCentroid(node->right, filteredCandidates[0]);
          }
        }
      }
      else
      {
        assignCentroid(node->left, filteredCandidates[0]);
        assignCentroid(node->right, filteredCandidates[0]);
      }
    }
    else
    {
      if (can_parallelize)
      {
        #pragma omp parallel
        {
          #pragma omp single
          {
            // Create tasks to filter the left and right subtrees recursively
            #pragma omp task shared(filteredCandidates)
            filterRecursive(node->left, filteredCandidates, depth + 1);

            #pragma omp task shared(filteredCandidates)
            filterRecursive(node->right, filteredCandidates, depth + 1);
          }
        }
      }
      else
      {
        filterRecursive(node->left, filteredCandidates, depth + 1);
        filterRecursive(node->right, filteredCandidates, depth + 1);
      }
    }
  }
}

// Finds the closest candidate to a target point
template <typename PT, std::size_t PD, class M>
std::shared_ptr<CentroidPoint<PT, PD>> KMeans<PT, PD, M>::findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target)
{
  auto closest = candidates[0];
  double minDist = metric.distanceTo(*closest, target);

  for (const auto &candidate : candidates)
  {
    double dist = metric.distanceTo(*candidate, target);
    if (dist < minDist)
    {
      minDist = dist;
      closest = candidate;
    }
  }
  return closest;
}

/** Implements the isFarther fuction discussed in the paper.
 *  Checks if a point z is farther from a bounding box than zStar
 */
template <typename PT, std::size_t PD, class M>
bool KMeans<PT, PD, M>::isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node)
{
  Point<PT, PD> u = z - zStar;

  Point<PT, PD> vH;
  for (std::size_t i = 0; i < PD; ++i)
  {
    vH.setValue((u.getValues()[i] >= 0) ? node.cellMax[i] : node.cellMin[i], i);
  }

  double distZ = metric.distanceTo(z, vH);
  double distZStar = metric.distanceTo(zStar, vH);

  return distZ > distZStar;
}

/**
 * Assign the centroid to the leaf nodes of the subtree
 */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::assignCentroid(std::unique_ptr<KdNode<PT, PD>> &node, const std::shared_ptr<CentroidPoint<PT, PD>> &centroid)
{
  if (!node->left && !node->right)
  {
    node->myPoint->setCentroid(centroid);
    return;
  }
  else
  {
    assignCentroid(node->left, centroid);
    assignCentroid(node->right, centroid);
  }
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