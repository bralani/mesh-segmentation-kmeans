#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <random>

#include "typedefs.h"
#include "CSVUtils.hpp"
#include "Point.hpp"
#include "CentroidPoint.hpp"
#include "KDTree.hpp"
#include <omp.h>

#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

/** Class implementing the K-Means algorithm using a Kd-Tree data structure
    and employing the filtering method discussed in the Paper */
template <typename PT, std::size_t PD>
class KMeans
{
public:
  /** Constructor
   * clusters: Number of clusters to be generated
   * points: Vector of Points to be used in K-Means
   * dist: Function used as the distance metric between two points
   */
  KMeans(int clusters, std::vector<Point<PT, PD>> &points, DistanceMetric &dist, PT treshold)
      : numClusters(clusters), data(points), metric(dist), treshold(treshold)
  {
    initializeCentroids();
    kdtree = new KdTree<PT, PD>(data);
  }

  // Destructor: deallocates the tree
  virtual ~KMeans() = default;

  // Fit method to perform the KMeans algorithm
  void fit();

  // Plot method
  void plot();

  void print();

private:
  int numClusters;                                 // Number of clusters
  std::vector<Point<PT, PD>> data;                 // Data points
  std::vector<CentroidPoint<PT, PD>> centroids;    // Centroids of clusters
  std::vector<CentroidPoint<PT, PD>> oldCentroids; // Centroids on the previous filter
  DistanceMetric &metric;                          // Distance metric function
  KdTree<PT, PD> *kdtree;                          // Kd-Tree structure
  PT treshold;

  // Method to extract randomly some initial clusters
  void initializeCentroids();

  // Filters the given centers based on the KD-tree structure
  void filter();

  // Recursive helper for the filter function
  void filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, int depth = 0);

  // Determines if a point `z` is farther from a bounding box than `zStar`
  bool isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node);

  // Finds the closest candidate (centroid) to a given point
  std::shared_ptr<CentroidPoint<PT, PD>> findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target);

  void assignCentroid(std::unique_ptr<KdNode<PT, PD>> &node, std::shared_ptr<CentroidPoint<PT, PD>> &centroid);

  bool checkConvergence();
};

/** Extracts randomly "numClusters" initial Centroids from the same data that were provided
 */
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::initializeCentroids()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  // Contenitore per memorizzare gli indici casuali
  std::vector<int> indices(data.size());
  std::iota(indices.begin(), indices.end(), 0); // Riempie gli indici da 0 a data.size() - 1

  // Selezione casuale senza ripetizioni
  std::vector<int> sampledIndices;
  std::sample(indices.begin(), indices.end(), std::back_inserter(sampledIndices), numClusters, gen);

  // Creazione dei centroidi iniziali
  for (int idx : sampledIndices)
  {
    centroids.push_back(CentroidPoint<PT, PD>(data[idx]));
  }
}

/** Fits the KMeans algorithm to the data */
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::fit()
{
  bool convergence = false;
  while (!convergence)
  {
    this->filter();
    convergence = checkConvergence();
    oldCentroids = centroids;
  }
}

template <typename PT, std::size_t PD>
bool KMeans<PT, PD>::checkConvergence()
{
  if (oldCentroids.empty())
    return false;
  else
  {
    PT dist = 0;
    for (int i = 0; i < centroids.size(); i++)
    {
      dist += centroids[i].distanceTo(oldCentroids[i], metric);
    }
    dist = dist / centroids.size();
    std::cout << dist << std::endl;
    if (dist > treshold)
      return false;
    return true;
  }
}

/** Implements the "filter" function discussed in the paper*/
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::filter()
{

  // Convert centers to shared pointers. During filterRecursive their values will be modified
  std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> centersPointers;
  for (CentroidPoint<PT, PD> &z : centroids)
  {
    z.resetCount();
    centersPointers.push_back(std::shared_ptr<CentroidPoint<PT, PD>>(&z, [](CentroidPoint<PT, PD> *) {}));
  }

  // Start the recursive filtering process
  filterRecursive(kdtree->getRoot(), centersPointers, 0);

  /** We need to divide the wgtCent of the centroid for the number of points which has it as centroid */
  for (CentroidPoint<PT, PD> &c : centroids)
  {
    c.normalize();
  }
}

/**  Recursive function for filtering. Follow exactly the paper */
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, int depth)
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

    if (filteredCandidates.size() == 1)
    {
      // If an internal node has a single candidate, just update it and spread the candidate among the subtree
      *filteredCandidates[0] = *filteredCandidates[0] + *node;
      assignCentroid(node->left, filteredCandidates[0]);
      assignCentroid(node->right, filteredCandidates[0]);
    }
    else
    {
      // Recursively filter left and right subtrees
      filterRecursive(node->left, filteredCandidates);
      filterRecursive(node->right, filteredCandidates);
    }
  }
}

// Finds the closest candidate to a target point
template <typename PT, std::size_t PD>
std::shared_ptr<CentroidPoint<PT, PD>> KMeans<PT, PD>::findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target)
{
  auto closest = candidates[0];
  double minDist = closest->distanceTo(target, metric);

  for (const auto &candidate : candidates)
  {
    double dist = candidate->distanceTo(target, metric);
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
template <typename PT, std::size_t PD>
bool KMeans<PT, PD>::isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node)
{
  Point<PT, PD> u = z - zStar;

  Point<PT, PD> vH;
  for (std::size_t i = 0; i < PD; ++i)
  {
    vH.setValue((u.getValues()[i] >= 0) ? node.cellMax[i] : node.cellMin[i], i);
  }

  double distZ = z.distanceTo(vH, metric);
  double distZStar = zStar.distanceTo(vH, metric);

  return distZ > distZStar;
}

/**
 * Assign the centroid to the leaf nodes of the subtree
 */
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::assignCentroid(std::unique_ptr<KdNode<PT, PD>> &node, std::shared_ptr<CentroidPoint<PT, PD>> &centroid)
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
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::print()
{
  std::cout << "-----------------------" << std::endl;
  std::cout << "Centroids: \n";
  for (auto &p : centroids)
  {
    p.print();
    std::cout << "\n";
  }

  try
  {
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