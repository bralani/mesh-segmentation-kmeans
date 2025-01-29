#ifndef GEODESICMETRIC_HPP
#define GEODESICMETRIC_HPP

#include <queue>
#include <utility>
#include <optional>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <omp.h>
#include "geometry/mesh/Mesh.hpp"
#include "geometry/metrics/Metric.hpp"
#include "geometry/point/CentroidPoint.hpp"

template <typename PT, std::size_t PD>
class GeodesicMetric : public Metric<PT, PD>
{
public:
    GeodesicMetric(Mesh &mesh, double percentage_threshold, std::vector<Point<PT, PD>> data);

    void setup() override;
    void fit_cpu() override;

#ifdef USE_CUDA
    void fit_gpu() override;
#endif

    FaceId findClosestFace(const Point<PT, PD> &centroid) const;

    std::vector<Point<PT, PD>>& getPoints() override;

protected:
    Mesh *mesh;
    std::unordered_map<FaceId, std::vector<PT>> distances;
    int oldPoints = 0;

    double computeEuclideanDistance(const Point<PT, PD>& a, const Point<PT, PD>& b) const;

    virtual std::vector<PT> computeDistances(const FaceId startFace) const;

    void storeCentroids() override ;
};

#endif // GEODESICMETRIC_HPP
