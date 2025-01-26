#include "geometry/metrics/GeodesicMetric.hpp"

template <typename PT, std::size_t PD>
GeodesicMetric<PT, PD>::GeodesicMetric(Mesh &mesh, double percentage_threshold)
    : mesh(&mesh)
{
  this->threshold = percentage_threshold;
}

template <typename PT, std::size_t PD>
void GeodesicMetric<PT, PD>::setup()
{
#pragma omp parallel for
  for (int centroidId = 0; centroidId < this->centroids->size(); ++centroidId)
  {
    const auto &centroid = this->centroids->at(centroidId);
    FaceId closestFaceId = findClosestFace(centroid);
    std::vector<PT> current_distances = computeDistances(closestFaceId);
    this->distances[FaceId(centroidId)] = current_distances;
  }
}

template <typename PT, std::size_t PD>
FaceId GeodesicMetric<PT, PD>::findClosestFace(const Point<PT, PD> &centroid) const
{
  double minDistance = std::numeric_limits<double>::max();
  FaceId closestFaceId = 0;

#pragma omp parallel
  {
    double threadMinDistance = std::numeric_limits<double>::max();
    FaceId threadClosestFaceId = 0;

#pragma omp for nowait
    for (FaceId faceId = 0; faceId < mesh->numFaces(); ++faceId)
    {
      const auto &face = mesh->getFace(faceId);
      const auto &baricenter = face.baricenter;

      double distance = computeEuclideanDistance(centroid, baricenter);

      if (distance < threadMinDistance)
      {
        threadMinDistance = distance;
        threadClosestFaceId = faceId;
      }
    }

#pragma omp critical
    {
      if (threadMinDistance < minDistance)
      {
        minDistance = threadMinDistance;
        closestFaceId = threadClosestFaceId;
      }
    }
  }

  return closestFaceId;
}

template <typename PT, std::size_t PD>
void GeodesicMetric<PT, PD>::fit_cpu()
{
  if (this->centroids->empty())
  {
    throw std::runtime_error("Centroids not set!");
  }

  const size_t numFaces = mesh->numFaces();
  const size_t numCentroids = this->centroids->size();

  std::vector<Point<PT, PD>> newCentroids(numCentroids);

  bool hasConverged = false;
  size_t iteration = 0;

  mesh->buildFaceAdjacency();

  while (!hasConverged)
  {
    unsigned int numChanged = 0;
    hasConverged = true;

    setup();

    for (FaceId faceId = 0; faceId < numFaces; ++faceId)
    {
      double minDistance = std::numeric_limits<double>::max();
      int closestCentroid = -1;

      for (size_t centroidIndex = 0; centroidIndex < numCentroids; ++centroidIndex)
      {
        double distance = this->distances[FaceId(centroidIndex)][faceId];
        if (distance < minDistance)
        {
          minDistance = distance;
          closestCentroid = centroidIndex;
        }
      }

      if (mesh->getFaceCluster(faceId) != closestCentroid)
      {
        numChanged++;
        mesh->setFaceCluster(faceId, closestCentroid);
      }
    }

    std::vector<size_t> counts(numCentroids, 0);
    for (size_t i = 0; i < numCentroids; ++i)
    {
      newCentroids[i].coordinates.fill(0);
    }

    for (FaceId faceId = 0; faceId < numFaces; ++faceId)
    {
      int centroidIndex = mesh->getFaceCluster(faceId);
      const auto &baricenter = mesh->getFace(faceId).baricenter;

      for (size_t dim = 0; dim < PD; ++dim)
      {
        newCentroids[centroidIndex].coordinates[dim] += baricenter.coordinates[dim];
      }
      counts[centroidIndex]++;
    }

    for (size_t centroidIndex = 0; centroidIndex < numCentroids; ++centroidIndex)
    {
      if (counts[centroidIndex] > 0)
      {
        for (size_t dim = 0; dim < PD; ++dim)
        {
          newCentroids[centroidIndex].coordinates[dim] /= counts[centroidIndex];
        }
      }
    }

    for (size_t i = 0; i < numCentroids; ++i)
    {
      this->centroids->at(i).coordinates = newCentroids[i].coordinates;
    }

    double curr_perc = static_cast<double>(numChanged) / numFaces;
    if (curr_perc > this->threshold)
    {
      hasConverged = false;
    }

    iteration++;
    if (iteration > 100)
    {
      std::cerr << "Warning: K-Means did not converge after 100 iterations." << std::endl;
      break;
    }
  }

  std::cout << "K-Means converged after " << iteration << " iterations." << std::endl;
}

template <typename PT, std::size_t PD>
double GeodesicMetric<PT, PD>::computeEuclideanDistance(const Point<PT, PD> &a, const Point<PT, PD> &b) const
{
  double sum = 0.0;
  for (std::size_t i = 0; i < PD; ++i)
  {
    sum += std::pow(a.coordinates[i] - b.coordinates[i], 2);
  }
  return std::sqrt(sum);
}

template <typename PT, std::size_t PD>
std::vector<PT> GeodesicMetric<PT, PD>::computeDistances(const FaceId startFace) const
{

  // Initialize Dijkstra's algorithm
  std::vector<PT> curr_distances(mesh->numFaces()); // Minimum distance from startFace
  std::unordered_map<FaceId, bool> visited;         // Keep track of visited faces
  std::priority_queue<std::pair<PT, FaceId>, std::vector<std::pair<PT, FaceId>>, std::greater<>> pq;

  // Initialize curr_distances and visited flags
  for (int i = 0; i < mesh->numFaces(); ++i)
  {
    curr_distances[i] = std::numeric_limits<PT>::max();
    visited[i] = false;
  }

  curr_distances[startFace] = 0;
  pq.push({0, startFace});

  // Execute Dijkstra
  while (!pq.empty())
  {
    auto [currentDistance, currentFace] = pq.top();
    pq.pop();

    // Check if the face has already been visited
    if (visited[currentFace])
      continue;
    visited[currentFace] = true;

    // Iterate over the neighbors of the current face
    for (const auto &neighbor : mesh->getFaceAdjacencyAt(currentFace))
    {
      const auto &neighborFace = mesh->getFace(neighbor);

      // Compute distance between baricenters
      const auto &currentBaricenter = mesh->getFace(currentFace).baricenter;
      const auto &neighborBaricenter = neighborFace.baricenter;
      PT weight = computeEuclideanDistance(currentBaricenter, neighborBaricenter);

      // Update the distance if a shorter path is found
      if (curr_distances[currentFace] + weight < curr_distances[neighbor])
      {
        curr_distances[neighbor] = curr_distances[currentFace] + weight;
        pq.push({curr_distances[neighbor], neighbor});
      }
    }
  }

  return curr_distances;
}

#ifdef USE_CUDA
template <typename PT, std::size_t PD>
void GeodesicMetric<PT, PD>::fit_gpu()
{
  // GPU implementation placeholder
}
#endif

// Explicit template instantiations
template class GeodesicMetric<double, 3>;
