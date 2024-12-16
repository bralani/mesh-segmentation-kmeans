
#include <omp.h>
#include <iostream>
#include <vector>
#include <chrono>
#include "../include/CSVUtils.hpp"
#include "../include/Point.hpp"
#include "../include/CentroidPoint.hpp"
#include "../include/KDTree.hpp"
#include "../include/KMeans.hpp"
#include "../include/metrics.hpp"
#include <random>


#define DIMENSION 2

template <typename PT, size_t PD>
std::vector<Point<PT, PD>> generateRandomPoints(int numPoints, double minValue, double maxValue) {
    std::vector<Point<PT, PD>> points;
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<> dis(minValue, maxValue);

    for (int i = 0; i < numPoints; ++i) {
        std::array<double, PD> coords;
        for (size_t d = 0; d < PD; ++d) {
            coords[d] = dis(gen);
        }
        points.emplace_back(coords);
    }

    return points;
}

int main() {
    std::vector<int> threadCounts = {1, 2, 3, 4, 5, 6}; // Test with these thread counts
    std::vector<Point<double, DIMENSION>> data = CSVUtils::readCSV<double, DIMENSION>("../resources/file_2d.csv");
    DistanceMetric euclMetric = euclideanMetric;
    int num_clusters = 2;
    omp_set_nested(1);

    std::cout<<"Possibili thread disponibili: " << std::thread::hardware_concurrency() <<std::endl;
    std::cout<<"Massimi thread di openMP: " << omp_get_max_threads() <<std::endl;

    auto points = generateRandomPoints<double, 2>(8000, -100, 100);

    for (int numThreads : threadCounts) {
        // Set the number of threads
        omp_set_num_threads(numThreads);
        
        std::cout << "Threads: " << numThreads << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        KMeans<double, DIMENSION> kmeans(num_clusters, points, euclMetric, 1e-4);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        
        std::cout<<"Time tree creation: " << elapsed.count() << " seconds.\n";

        auto start1 = std::chrono::high_resolution_clock::now();
        kmeans.fit();
        auto end1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed1 = end1 - start1;

        std::cout<<"Time tree fit: " << elapsed1.count() << " seconds.\n";
    }

    return 0;
}
