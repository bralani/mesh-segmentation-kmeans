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

template<std::size_t PD>
class MostDistanceClass : public CentroidInitMethod<double, PD> {
public:
    MostDistanceClass(const std::vector<Point<double, PD>>& data, int k);
    MostDistanceClass(const std::vector<Point<double, PD>>& data);

    void findCentroid(std::vector<CentroidPoint<double, PD>>& centroids) override;

};


#endif
