#ifndef SILHOUETTE
#define SILHOUETTE

#include <cstddef> 
#include <vector>
#include <cmath>
#include "clustering/CentroidInitializationMethods/kInitMethods.hpp"
#include "clustering/CentroidInitializationMethods/MostDistantCentroids.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"

#define MAX_CLUSTER 10

// Forward declaration di KMeans per evitare dipendenze circolari
template <typename PT, std::size_t PD, class M>
class KMeans;

// Classe SilhouetteMethod che eredita da Kinit
template<typename PT, std::size_t PD, class M>
class SilhouetteMethod : public Kinit<PT, PD, M> {
public: 
    // Costruttore che inizializza la classe con un riferimento a KMeans
    SilhouetteMethod(const KMeans<PT, PD, M>& kMeans) : Kinit<PT, PD, M>(kMeans) {}

    // Metodo per trovare il numero ottimale di cluster
    int findK();
    
private:
    // Funzione per calcolare il Silhouette Score per un dato valore di k
    double computeSilhouetteScore(int k);
};

// Implementazione del metodo findK()
template<typename PT, std::size_t PD, class M>
int SilhouetteMethod<PT, PD, M>::findK() {
    int optimalK = 2; // Silhouette non ha senso per k=1
    double maxSilhouette = -1.0;

    // Recupera i punti dal KMeans
    std::vector<Point<PT, PD>> points = (this->m_kMeans).getPoints();
    std::cout << "Start searching k using Silhouette Method...\n";

    for (int k = 2; k < MAX_CLUSTER; ++k) {
        double silhouette = computeSilhouetteScore(k);
        std::cout<<"Silhouette score " << silhouette << ", with k = " << k <<std::endl;
        if (silhouette > maxSilhouette) {
            maxSilhouette = silhouette;
            optimalK = k;
        }
    }

    std::cout << "Optimal k found: " << optimalK << std::endl;
    return optimalK;
}

// Implementazione della funzione per calcolare il Silhouette Score
template<typename PT, std::size_t PD, class M>
double SilhouetteMethod<PT, PD, M>::computeSilhouetteScore(int k) {
    //Initialization
    std::vector<Point<PT, PD>> points = (this->m_kMeans).getPoints();
    MostDistanceClass mdc(points, k);  
    std::vector<CentroidPoint<PT, PD>>& pointerCentroids = (this->m_kMeans).getCentroids();

    //Execution 
    mdc.findCentroid(pointerCentroids);
    (this->m_kMeans).setNumClusters(static_cast<std::size_t>(k));
    (this->m_kMeans).fit();
            
    points = (this->m_kMeans).getPoints();

    double totalScore = 0.0;
    int numPoints = points.size();

    for (int i = 0; i < numPoints; ++i) {
        double a = 0.0, c = 0.0;
        int countA = 0, countC = 0;

        // a(i) is the average distance between i and all the other data points in the cluster to which i belongs.
        for (countA = 0; countA < numPoints; ++countA) {
            if(i != countA &&  *(points[countA].centroid) == *(points[i].centroid) ){
                a += EuclideanMetric<PT, PD>::distanceTo(points[countA], points[i]);
            }
        }
        if (countA > 0) a /= countA;

        //b(i) is the average distance from i to all clusters to which i does not belong.
        for (countC = 0; countC < pointerCentroids.size(); ++countC) {
            if( pointerCentroids[countC] != *(points[i].centroid) ){
                c += EuclideanMetric<PT, PD>::distanceTo( pointerCentroids[countC] , points[i]);
            }
        }
        if (countC > 0) c /= countC;
        
        // S(i) is the silhouette coefficient of the data point i.
        totalScore += (c - a) / std::max(a, c);
    }

    return totalScore / numPoints;
}

#endif // SILHOUETTE
