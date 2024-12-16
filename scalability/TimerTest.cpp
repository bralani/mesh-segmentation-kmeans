
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


#define DIMENSION 2


int main() {
    std::vector<int> threadCounts = {1, 2, 4, 8, 16, 32}; // Test with these thread counts
    std::vector<Point<double, DIMENSION>> data = CSVUtils::readCSV<double, DIMENSION>("../resources/file_2d.csv");
    DistanceMetric euclMetric = euclideanMetric;


    for (int numThreads : threadCounts) {
        // Set the number of threads
        omp_set_num_threads(numThreads);
        std::cout << "Threads: " << numThreads << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        KMeans<double, DIMENSION> kmeans(2, data, euclMetric);
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
