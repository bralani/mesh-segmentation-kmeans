#include "geometry/metrics/Metric.hpp"

template <typename PT, std::size_t PD>
Metric<PT, PD>::Metric() : centroids(nullptr) {}

template <typename PT, std::size_t PD>
Metric<PT, PD>::Metric(std::vector<CentroidPoint<PT, PD>> &centroids) : centroids(&centroids) {}

template <typename PT, std::size_t PD>
Metric<PT, PD>::Metric(std::vector<CentroidPoint<PT, PD>> &centroids, std::vector<Point<PT, PD>> data) : centroids(&centroids), data(data) {}

template <typename PT, std::size_t PD>
void Metric<PT, PD>::setPoints(std::vector<Point<PT, PD>> data){
    this->data = data;
}

template <typename PT, std::size_t PD>
std::vector<Point<PT, PD>>& Metric<PT, PD>::getPoints(){
    return this->data;
}

template <typename PT, std::size_t PD>
Metric<PT, PD>::~Metric() = default;

template <typename PT, std::size_t PD>
void Metric<PT, PD>::setCentroids(std::vector<CentroidPoint<PT, PD>> &centroids) {
    this->centroids = &centroids;
}

template <typename PT, std::size_t PD>
void Metric<PT, PD>::resetCentroids() {
    oldCentroids.clear();
    oldCentroids.shrink_to_fit();
    for(Point<PT, PD> &p : this->data){
        p.centroid.reset();
    }
}

template class Metric<double, 2>;
template class Metric<double, 3>;