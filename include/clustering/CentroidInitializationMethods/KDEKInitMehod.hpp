#ifndef KDE_K_INIT
#define KDE_K_INIT

#include <cstddef> 

// Forward declaration of KMeans to avoid cyclic dependencies
template <typename PT, std::size_t PD, class M>
class KMeans;

// Forward declaration of Kinit, the base class for K initialization methods
template<typename PT, std::size_t PD, class M>
class Kinit;

// Forward declaration of KDE, a class for kernel density estimation
template<std::size_t PD>
class KDE;

// KDEMethod class: Implements a K initialization method based on KDE (Kernel Density Estimation)
template<typename PT, std::size_t PD, class M>
class KDEMethod : public Kinit<PT, PD, M> {

    public: 
        // Constructor: Initializes KDEMethod with a reference to the KMeans object
        KDEMethod(const KMeans<PT, PD, M>& kMeans) : Kinit<PT, PD, M>(kMeans) {}

        // Method to find the optimal number of clusters (K) using KDE
        int findK();
};

// Implementation of the findK method
template<typename PT, std::size_t PD, class M>
int KDEMethod<PT, PD, M>::findK() {
    // Retrieve the points from the KMeans object
    std::vector<Point<PT, PD>> points = (this->m_kMeans).getPoints();

    // Create a KDE object using the retrieved points
    KDE kde(points);

    // Use KDE to find the optimal number of clusters without restrictions
    return kde.findLocalWithoutRestriction();
}

#endif
