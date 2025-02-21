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

/**
 * \class KDEMethod
 * \brief Implements a K initialization method based on Kernel Density Estimation (KDE).
 * 
 * This class provides functionality to find the optimal number of clusters (K) for the KMeans
 * algorithm using Kernel Density Estimation, helping in improving the initialization step of KMeans.
 * 
 * \tparam PT Type representing a point in the dataset (e.g., a vector of values).
 * \tparam PD The dimensionality of the points.
 * \tparam M The type of the model or metric used in the KMeans algorithm.
 */
template<typename PT, std::size_t PD, class M>
class KDEMethod : public Kinit<PT, PD, M> {

public: 
    /**
     * \brief Constructor for the KDEMethod class.
     * 
     * Initializes the KDEMethod object with a reference to a KMeans object.
     * This constructor sets up the necessary data for KDE-based K initialization.
     *
     * \param kMeans A reference to the KMeans object used for clustering.
     */
    KDEMethod(const KMeans<PT, PD, M>& kMeans) : Kinit<PT, PD, M>(kMeans) {}

    /**
     * \brief Finds the optimal number of clusters (K) using Kernel Density Estimation (KDE).
     * 
     * This method uses KDE to estimate the optimal number of clusters by analyzing the distribution
     * of data points and identifying the appropriate number of density peaks.
     * 
     * \return The optimal number of clusters, K.
     */
    int findK();
};

// Implementation of the findK method
template<typename PT, std::size_t PD, class M>
int KDEMethod<PT, PD, M>::findK() {
    // Retrieve the points from the KMeans object
    std::vector<Point<PT, PD>> points = (this->m_kMeans).getPoints();
    std::cout << "Searching K with KDE...";
    // Create a KDE object using the retrieved points
    KDE kde(points);
    int k = kde.findLocalWithoutRestriction();
    std::cout<< "Optimal is " << k << std::endl;
    // Use KDE to find the optimal number of clusters without restrictions
    return k;
}

#endif
