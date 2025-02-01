#ifndef ELBOW
#define ELBOW

#include <cstddef> 
#include "clustering/CentroidInitializationMethods/kInitMethods.hpp"

#define MAX_FACES 55644
#define MIN_FACES 2682

#define OUT_RANGE_MAX 500
#define OUT_RANGE_MIN 75

// Forward declaration of KMeans to avoid cyclic dependencies
template <typename PT, std::size_t PD, class M>
class KMeans;

// Forward declaration of Kinit to ensure it is available as a base class
template<typename PT, std::size_t PD, class M>
class Kinit;

// ElbowMethod class that inherits from Kinit
// Implements the "elbow method" to determine the optimal number of clusters
template<typename PT, std::size_t PD, class M>
class ElbowMethod : public Kinit<PT, PD, M> {
public: 
    // Constructor: Initializes ElbowMethod with a reference to KMeans
    ElbowMethod(const KMeans<PT, PD, M>& kMeans) : Kinit<PT, PD, M>(kMeans) {}

    // Method to determine the optimal number of clusters
    int findK();
    
private:
    // Stores the Within-Cluster Sum of Squares (WCSS) for each k
    std::vector<PT> wcss;
};

// Implementation of the findK method
template<typename PT, std::size_t PD, class M>
int ElbowMethod<PT, PD, M>::findK() {

    int k = 1; // Start with k = 1 cluster
    double epsilon = 1e-2; // Convergence threshold for WCSS change
    int optimalK = 0; // Variable to store the optimal number of clusters

    // Retrieve the points from KMeans
    std::vector<Point<PT, PD>> points = (this->m_kMeans).getPoints();
    int range = OUT_RANGE_MIN + (points.size() - MIN_FACES) * (OUT_RANGE_MAX - OUT_RANGE_MIN) / (MAX_FACES - MIN_FACES);
    std::cout << "Start searching k...\n";

    while (true) { // Infinite loop to incrementally search for the optimal k

        MostDistanceClass mdc(points, k); 
        std::vector<CentroidPoint<PT, PD>>& pointerCentroids = (this->m_kMeans).getCentroids();
        mdc.findCentroid(pointerCentroids);
        (this->m_kMeans).setNumClusters(static_cast<std::size_t>(k));
        (this->m_kMeans).fit();
        points = (this->m_kMeans).getPoints();
        double sum = 0; 

        #pragma omp parallel for reduction(+:sum)
        for (size_t i = 0; i < points.size(); ++i) {
            Point<PT, PD> centroidTmp = *(points[i].centroid);
            sum += std::pow(EuclideanMetric<PT, PD>::distanceTo(points[i], centroidTmp), 2);
        }

        std::cout << "K: " << k << ", WCSS: " << sum << std::endl;
        (this->wcss).push_back(sum); 

        if (k > 2 && (std::abs(wcss[k-2] - wcss[k-1]) <= range || wcss[k-2] < wcss[k-1] )) { 
            optimalK = k; 
            break; 
        }

        (this->m_kMeans).resetCentroids();
        k++; 
    }
    return optimalK; // Return the optimal number of clusters
}

#endif
