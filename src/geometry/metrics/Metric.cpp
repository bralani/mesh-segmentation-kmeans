#include "geometry/metrics/Metric.hpp"

template <typename PT, std::size_t PD>
Metric<PT, PD>::Metric() : centroids(nullptr) {}

template <typename PT, std::size_t PD>
Metric<PT, PD>::Metric(std::vector<CentroidPoint<PT, PD>> &centroids) : centroids(&centroids) {}

template <typename PT, std::size_t PD>
Metric<PT, PD>::~Metric() = default;

template <typename PT, std::size_t PD>
void Metric<PT, PD>::setCentroids(std::vector<CentroidPoint<PT, PD>> &centroids) {
    this->centroids = &centroids;
}

template class Metric<double, 2>;
template class Metric<double, 3>;