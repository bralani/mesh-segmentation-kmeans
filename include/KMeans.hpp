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
#include "./matplotlib-cpp/matplotlibcpp.h"

// A class to implement the KMeans clustering algorithm
template <typename PT, std::size_t PD>
class KMeans
{
public:
  // Constructor: builds the kmeans object from a set of points, number of clusters, and a distance metric
  KMeans(int clusters, std::vector<Point<PT, PD>> &points, DistanceMetric &dist)
  : numClusters(clusters), data(points), metric(dist) {  // Initialize metric here
    initializeCentroids();
    kdtree = new KdTree<PT, PD>(points);
  }

  // Destructor: deallocates the tree
  ~KMeans() = default;

  // Fit method to perform the KMeans algorithm
  void fit();

  // Plot method
  void plot();

private:
  int numClusters;                      // Number of clusters
  std::vector<Point<PT, PD>> data;      // Data points
  std::vector<CentroidPoint<PT, PD>> centroids; // Centroids of clusters
  DistanceMetric &metric;               // Distance metric function
  KdTree <PT, PD>* kdtree;            // KD-Tree for efficient nearest neighbor search

  // Method to initialize centroids randomly
  void initializeCentroids();

  // Filters the given centers based on the KD-tree structure
  void filter(std::vector<CentroidPoint<PT, PD>>& centers);

  // Recursive helper for the filter function
  void filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates);

  // Finds the closest point to a target from a set of candidates
  Point<PT, PD> findClosestPoint(const std::vector<Point<PT, PD>> &candidates, const Point<PT, PD> &target);

  // Determines if a point `z` is farther from a bounding box than `zStar`
  bool isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node);

  // Finds the closest candidate (centroid) to a given point
  CentroidPoint<PT, PD> findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target);
};

// Initializes the centroids randomly
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::initializeCentroids()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, data.size() - 1);

  for (int i = 0; i < numClusters; ++i)
  {
    int idx = dist(gen);
    centroids.push_back(CentroidPoint<PT, PD>(data[idx]));
  }
}

// Fits the KMeans algorithm to the data
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::fit()
{
  this->filter(centroids);
}

// Filters the given set of centers using the KD-tree
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::filter(std::vector<CentroidPoint<PT, PD>> &centers)
{
  std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> centersPointers;

  // Convert centers to shared pointers for in-place modification
  for (CentroidPoint<PT, PD> &z : centers)
  {
    centersPointers.push_back(std::make_shared<CentroidPoint<PT, PD>>(z));
  }

  // Start the recursive filtering process
  filterRecursive(kdtree->getRoot(), centersPointers);
}

// Finds the closest candidate to a target point
template <typename PT, std::size_t PD>
CentroidPoint<PT, PD> KMeans<PT, PD>::findClosestCandidate(const std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates, const Point<PT, PD> &target)
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
  return *closest;
}

// Recursive function for filtering
template <typename PT, std::size_t PD>
void KMeans<PT, PD>::filterRecursive(std::unique_ptr<KdNode<PT, PD>> &node, std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> &candidates)
{
  if (!node)
    return;

  // Leaf node: find the closest candidate and update it
  if (!node->left && !node->right)
  {
    CentroidPoint<PT, PD> zStar = findClosestCandidate(candidates, node->wgtCent);
    zStar = zStar + node->wgtCent;
    zStar.count += node->count;
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
    CentroidPoint<PT, PD> zStar = findClosestCandidate(candidates, cellMidpoint);

    // Filter candidates based on proximity to zStar
    std::vector<std::shared_ptr<CentroidPoint<PT, PD>>> filteredCandidates;
    filteredCandidates.reserve(candidates.size());

    // Recursively filter left and right subtrees
    if (filteredCandidates.size() == 1)
    {
      *filteredCandidates[0] = *filteredCandidates[0] + node->wgtCent;
      filteredCandidates[0]->count = filteredCandidates[0]->count + node->count;
    }
    else
    {
      filterRecursive(node->left, filteredCandidates);
      filterRecursive(node->right, filteredCandidates);
    }
  }
}

// Finds the closest point to the target among a set of candidates
template <typename PT, std::size_t PD>
Point<PT, PD> KMeans<PT, PD>::findClosestPoint(const std::vector<Point<PT, PD>> &candidates, const Point<PT, PD> &target)
{
  Point<PT, PD> closest = candidates[0];
  double minDist = closest.distanceTo(target);

  for (const Point<PT, PD> &z : candidates)
  {
    double dist = z.distanceTo(target);
    if (dist < minDist)
    {
      minDist = dist;
      closest = z;
    }
  }

  return closest;
}

// Checks if a point z is farther from a bounding box than zStar
template <typename PT, std::size_t PD>
bool KMeans<PT, PD>::isFarther(const Point<PT, PD> &z, const Point<PT, PD> &zStar, const KdNode<PT, PD> &node)
{
  Point<PT, PD> u = z - zStar;

  Point<PT, PD> vH;
  for (std::size_t i = 0; i < PD; ++i)
  {
    vH.getValues()[i] = (u.getValues()[i] > 0) ? node.cellMax[i] : node.cellMin[i];
  }

  double distZ = z.distanceTo(vH);
  double distZStar = zStar.distanceTo(vH);

  return distZ > distZStar;
}