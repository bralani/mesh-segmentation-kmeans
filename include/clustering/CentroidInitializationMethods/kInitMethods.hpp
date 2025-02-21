/*============================================================================
 *  Kinit.hpp
 *============================================================================*/

 #ifndef KINIT_HPP
 #define KINIT_HPP
 
 #include <cstddef> 
 
 #include "geometry/metrics/Metric.hpp"
 #include "geometry/metrics/GeodesicMetric.hpp"
 #include "geometry/metrics/GeodesicHeatMetric.hpp"
 #include "geometry/metrics/EuclideanMetric.hpp"
 
 template <typename PT, std::size_t PD, class M>
 class KMeans;
 
 /**
  * \class Kinit
  * \brief Abstract base class for determining the optimal number of clusters (k).
  *
  * This class provides an interface for algorithms that determine the optimal 
  * number of clusters (k) in a dataset. It includes a pure virtual method `findK()` 
  * that must be implemented in derived classes.
  *
  * \tparam PT Type of the points (e.g., float, double, etc.).
  * \tparam PD Dimension of the data points.
  */
 template<typename PT, std::size_t PD, class M>
 class Kinit {
 public:
     /**
      * \brief Constructor for the Kinit class.
      *
      * \param kMeans Reference to a KMeans object.
      */
     Kinit(const KMeans<PT, PD, M>& kMeans) : m_kMeans(kMeans) {
         // Do nothing
     }
 
     /**
      * \brief Virtual destructor for safe polymorphic usage.
      */
     virtual ~Kinit() = default;
 
     /**
      * \brief Pure virtual function to determine the number of clusters (k).
      *
      * Derived classes must implement this method to calculate the optimal number of clusters.
      *
      * \return The optimal number of clusters (k).
      */
     virtual int findK() = 0;
 
 protected:
     KMeans<PT, PD, M> m_kMeans; ///< KMeans instance used for clustering.
 };
 
 #endif // KINIT_HPP
 
