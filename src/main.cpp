#include <vector>

#include <iostream>
#include "CSVUtils.hpp"
#include "../include/CSVUtils.hpp"
#include "../include/metrics.hpp"
#include "../include/KMeans.hpp"

#define DIMENSION 2

using namespace std;


int main()
{
    string path;
    cout << "Enter the path to the file csv: ";
    cin >> path;

    int num_clusters;
    cout << "Enter the number of clusters (parameter k): ";
    cin >> num_clusters;


    // Reading from the file
    std::vector<Point<double, DIMENSION>> data = CSVUtils::readCSV<double, DIMENSION>(path);

    DistanceMetric euclMetric = euclideanMetric;
    KMeans<double, DIMENSION> kmeans(num_clusters, data, euclMetric);

    kmeans.fit();

    return 0;
}
