#ifndef SILHOUETTE
#define SILHOUETTE

#include <cstddef>
#include <vector>
#include <cmath>
#include "clustering/CentroidInitializationMethods/kInitMethods.hpp"
#include "clustering/CentroidInitializationMethods/MostDistantCentroids.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"

#define MAX_CLUSTER 10

// Forward declaration of KMeans to avoid circular dependencies
template <typename PT, std::size_t PD, class M>
class KMeans;

/**
 * @brief Implements the Silhouette Method to determine the optimal number of clusters.
 *
 * This class inherits from Kinit and uses the silhouette coefficient to find the
 * best k for K-Means clustering.
 */
template <typename PT, std::size_t PD, class M>
class SilhouetteMethod : public Kinit<PT, PD, M>
{
public:
    /**
     * @brief Constructor for the SilhouetteMethod.
     * @param kMeans Reference to the KMeans object.
     */
    SilhouetteMethod(const KMeans<PT, PD, M> &kMeans) : Kinit<PT, PD, M>(kMeans) {}

    /**
     * @brief Determines the optimal number of clusters using the Silhouette Score.
     * @return The optimal number of clusters.
     */
    int findK();

private:
    /**
     * @brief Computes the Silhouette Score for a given k.
     * @param k The number of clusters to evaluate.
     * @return The silhouette score for k clusters.
     */
    double computeSilhouetteScore(int k);
};

// Implementation of the findK method
template <typename PT, std::size_t PD, class M>
int SilhouetteMethod<PT, PD, M>::findK()
{
    int optimalK = 2; // The silhouette method does not apply to k=1
    double maxSilhouette = -1.0;

    // Retrieve points from KMeans
    std::vector<Point<PT, PD>> points = (this->m_kMeans).getPoints();
    std::cout << "Start searching k using Silhouette Method...\n";

    // Iterate over different k values to find the one with the best silhouette score
    for (int k = 2; k < MAX_CLUSTER; ++k)
    {
        double silhouette = computeSilhouetteScore(k);
        std::cout << "Silhouette score " << silhouette << ", with k = " << k << std::endl;
        if (silhouette > maxSilhouette)
        {
            maxSilhouette = silhouette;
            optimalK = k;
        }
    }

    std::cout << "Optimal k found: " << optimalK << std::endl;
    return optimalK;
}

// Implementation of the computeSilhouetteScore method
template <typename PT, std::size_t PD, class M>
double SilhouetteMethod<PT, PD, M>::computeSilhouetteScore(int k)
{
    // Initialization
    std::vector<Point<PT, PD>> points = (this->m_kMeans).getPoints();
    MostDistanceClass mdc(points, k);
    std::vector<CentroidPoint<PT, PD>> &pointerCentroids = (this->m_kMeans).getCentroids();

    // Execute clustering with k clusters
    mdc.findCentroid(pointerCentroids);
    (this->m_kMeans).setNumClusters(static_cast<std::size_t>(k));
    (this->m_kMeans).fit();

    points = (this->m_kMeans).getPoints();

    double totalScore = 0.0;
    int numPoints = points.size();

/**
 * @brief Computes the Silhouette Score in parallel using OpenMP.
 *
 * Each thread processes a subset of points to compute the intra-cluster distance (a)
 * and the nearest-cluster distance (c), contributing to the total silhouette score.
 */
#pragma omp parallel for reduction(+ : totalScore)
    for (int i = 0; i < numPoints; ++i)
    {
        double a = 0.0, c = 0.0;
        int countA = 0, countC = 0;

        // Compute a(i): the average distance from point i to all other points in the same cluster
        for (int j = 0; j < numPoints; ++j)
        {
            if (i != j && *(points[j].centroid) == *(points[i].centroid))
            {
                a += EuclideanMetric<PT, PD>::distanceTo(points[j], points[i]);
                countA++;
            }
        }
        if (countA > 0)
            a /= countA; // Normalize a(i)

        // Compute c(i): the average distance from point i to the closest cluster centroid
        for (int j = 0; j < pointerCentroids.size(); ++j)
        {
            if (pointerCentroids[j] != *(points[i].centroid))
            {
                c += EuclideanMetric<PT, PD>::distanceTo(pointerCentroids[j], points[i]);
                countC++;
            }
        }
        if (countC > 0)
            c /= countC; // Normalize c(i)

        // Compute S(i): silhouette coefficient for the data point i
        if (a > 0 || c > 0)
        {
            totalScore += (c - a) / std::max(a, c);
        }
    }

    // Return the average silhouette score over all points
    return totalScore / numPoints;
}

#endif // SILHOUETTE
