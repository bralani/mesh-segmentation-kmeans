/*============================================================================
 *  MostDistanceClass.hpp
 *============================================================================*/

 #ifndef MOST_DISTANCE_CLASS_HPP
 #define MOST_DISTANCE_CLASS_HPP
 
 #include <cstddef>
 #include <iostream>
 #include <random>
 #include <limits>
 #include <vector>
 #include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"
 
 template <typename PT, std::size_t PD>
 class Point;
 
 #define LIMIT_NUM_CENTROIDS 10
 
 /**
  * \class MostDistanceClass
  * \brief Class for centroid initialization based on maximum distance.
  *
  * This class inherits from CentroidInitMethod and implements a method to
  * select initial centroids based on maximum distance.
  *
  * \tparam PD Dimension of the point (e.g., 3D)
  */
 template<std::size_t PD>
 class MostDistanceClass : public CentroidInitMethod<double, PD> {
 public:
     /**
      * \brief Constructor for the MostDistanceClass.
      *
      * Initializes the algorithm with the provided data and number of centroids.
      *
      * \param data Vector of points used to compute centroids.
      * \param k Number of centroids to select.
      */
     MostDistanceClass(const std::vector<Point<double, PD>>& data, int k);
 
     /**
      * \brief Constructor for the MostDistanceClass with a predefined number of centroids.
      *
      * \param data Vector of points used to compute centroids.
      */
     MostDistanceClass(const std::vector<Point<double, PD>>& data);
 
     /**
      * \brief Finds the initial centroids based on maximum distance.
      *
      * This method overrides the virtual method of the base class to
      * select initial centroids.
      *
      * \param centroids Vector where the found centroids will be stored.
      */
     void findCentroid(std::vector<CentroidPoint<double, PD>>& centroids) override;
 };
 
 #endif // MOST_DISTANCE_CLASS_HPP