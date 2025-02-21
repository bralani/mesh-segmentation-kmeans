#include "clustering/KMeans.hpp"

template <typename PT, std::size_t PD, class M>
KMeans<PT, PD, M>::KMeans(std::size_t clusters, PT treshold,
                          M *metric, int centroidsInitializationMethod, int kInitializationMethod)
    : metric(metric), treshold(treshold), numClusters(clusters)
{
  initializeCentroids(centroidsInitializationMethod, kInitializationMethod);
}

template <typename PT, std::size_t PD, class M>
std::vector<Point<PT, PD>> &KMeans<PT, PD, M>::getPoints()
{
  return metric->getPoints();
}

template <typename PT, std::size_t PD, class M>
std::vector<CentroidPoint<PT, PD>> &KMeans<PT, PD, M>::getCentroids()
{
  return centroids;
}

/** Extracts randomly "numClusters" initial Centroids from the same data that were provided
 */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::initializeCentroids(int centroidsInitializationMethod, int kInitializationMethod)
{
  if (centroidsInitializationMethod < 0 || centroidsInitializationMethod > 3)
  {
    throw std::invalid_argument("Not a valid centroids initialization method!");
  }

  if (numClusters == 0)
  {
    std::unique_ptr<Kinit<PT, PD, M>> kinit;

    if (kInitializationMethod == Enums::KInit::ELBOW_METHOD)
      kinit = std::make_unique<ElbowMethod<PT, PD, M>>(*this);
    else if (kInitializationMethod == Enums::KInit::KDE_METHOD)
      kinit = std::make_unique<KDEMethod<PT, PD, M>>(*this);
    else if (kInitializationMethod == Enums::KInit::SILHOUETTE_METHOD)
      kinit = std::make_unique<SilhouetteMethod<PT, PD, M>>(*this);
    else
      throw std::invalid_argument("Invalid k initialization method");

    numClusters = kinit->findK(); // `unique_ptr` dealloca automaticamente alla fine del blocco
  }

  std::unique_ptr<CentroidInitMethod<double, PD>> cim;
  auto &points = metric->getPoints();

  if (centroidsInitializationMethod == Enums::CentroidInit::RANDOM)
    cim = std::make_unique<RandomCentroidInit<PT, PD>>(points, numClusters);
  else if (centroidsInitializationMethod == Enums::CentroidInit::KDE)
    cim = std::make_unique<KDE<PD>>(points, numClusters);
  else if (centroidsInitializationMethod == Enums::CentroidInit::MOSTDISTANT)
    cim = std::make_unique<MostDistanceClass<PD>>(points, numClusters);
  else if constexpr (PD == 3)
    cim = std::make_unique<KDE3D>(points, numClusters);
  else
    throw std::invalid_argument("Invalid centroids initialization method");

  cim->findCentroid(this->centroids);
}

template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::resetCentroids()
{
  centroids.clear();
  centroids.shrink_to_fit();
  (this->metric)->resetCentroids();
}

template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::setNumClusters(std::size_t numC)
{
  this->numClusters = numC;
}

/** Fits the KMeans algorithm to the data */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::fit()
{
  metric->setCentroids(centroids);

#ifdef USE_CUDA
  if (metric->getPoints().size() > MIN_NUM_POINTS_CUDA)
  {
    metric->fit_gpu();
  }
  else
  {
    metric->fit_cpu();
  }
#else
  metric->fit_cpu();
#endif
  CentroidInitMethod<PT, PD>::exportedMesh(centroids, "CentroidsFix");
}

/** DEBUG FUNCTION */
template <typename PT, std::size_t PD, class M>
void KMeans<PT, PD, M>::print()
{
  std::cout << "Fitting phase terminated" << std::endl;
  std::cout << "-----------------------" << std::endl;
  std::cout << "Centroids: \n";
  for (auto &p : centroids)
  {
    p.print();
    std::cout << "\n";
  }

  std::cout << "-----------------------" << std::endl;
  std::cout << "Points: \n";

  auto &points = metric->getPoints();

  // Color map based on the centroid index
  std::vector<std::string> colors = {"r", "g", "b", "y", "m", "c", "k", "orange", "purple", "brown"};

  // Create vectors for points associated with each centroid
  std::vector<std::vector<double>> x_points_per_centroid(centroids.size());
  std::vector<std::vector<double>> y_points_per_centroid(centroids.size());

  for (auto &p : points)
  {
    p.print();
    if (p.centroid != nullptr) // Check if a centroid is set
    {
      std::cout << " -> Centroid: ";
      p.centroid->print();

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
}

template class KMeans<double, 2, EuclideanMetric<double, 2>>;
template class KMeans<double, 3, EuclideanMetric<double, 3>>;
template class KMeans<double, 3, GeodesicHeatMetric<double, 3>>;
template class KMeans<double, 3, GeodesicDijkstraMetric<double, 3>>;
