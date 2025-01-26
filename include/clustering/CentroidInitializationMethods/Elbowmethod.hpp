#ifndef ELBOW
#define ELBOW

#include <cstddef> 
#include "clustering/CentroidInitializationMethods/kInitMethods.hpp"

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

    // Initialize a Euclidean metric for calculating distances
    EuclideanMetric<double, PD> metric((this->m_kMeans).getPoints(), 1e-4);
    // Retrieve the points from KMeans
    std::vector<Point<PT, PD>> points = (this->m_kMeans).getPoints();
    
    std::cout << "Start searching k...\n";

    while (true) { // Infinite loop to incrementally search for the optimal k

        // Initialize centroids using the most distant class method
        MostDistanceClass mdc(points, k); 
        // Get centroids from KMeans
        std::vector<CentroidPoint<PT, PD>> pointerCentroids = (this->m_kMeans).getCentroids();
        // Find centroids using the current method
        mdc.findCentroid(pointerCentroids);
        // Fit the KMeans model to the data
        (this->m_kMeans).fit();
        
        std::cout << "End fitting with k = " << k << "\n"; 

        double sum = 0; // Accumulate the sum of distances for WCSS

        // Calculate the sum of squared distances between points and their centroids
        for (const Point<double, PD>& d : points) {
            sum += metric.distanceTo(d, *(d.centroid));
        }

        std::cout << "K: " << k << ", WCSS: " << sum << std::endl;
        (this->wcss).push_back(sum); // Store the WCSS value for this k

        // If k > 2, check for convergence based on the relative change in WCSS
        if (k > 2) {
            double delta = (wcss[k - 2] - wcss[k - 1]) / wcss[k - 2];
            if (delta < epsilon) { // Convergence condition
                optimalK = k; // Store the optimal k
                break; // Exit the loop
            }
        }

        k++; // Increment k for the next iteration
    }
    return optimalK; // Return the optimal number of clusters
}

#endif
