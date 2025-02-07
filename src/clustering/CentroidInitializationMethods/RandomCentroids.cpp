
#include "clustering/CentroidInitializationMethods/RandomCentroids.hpp"
#include <random>
#include <algorithm>
#include <stdexcept>
#include <numeric>

template <typename PT, std::size_t PD>
RandomCentroidInit<PT, PD>::RandomCentroidInit(const std::vector<Point<PT, PD>>& data, int k)
    : CentroidInitMethod<PT, PD>(data, k) {}

template <typename PT, std::size_t PD>
RandomCentroidInit<PT, PD>::RandomCentroidInit(const std::vector<Point<PT, PD>>& data)
    : CentroidInitMethod<PT, PD>(data) {}

template <typename PT, std::size_t PD>
void RandomCentroidInit<PT, PD>::findCentroid(std::vector<CentroidPoint<PT, PD>>& centroids) {
    std::random_device rd;         // Random device for seeding
    std::mt19937 gen(rd());        // Mersenne Twister generator
    std::size_t datasize = this->m_data.size(); // Size of the dataset

    // If the number of clusters is not set, randomly select a number between 1 and 10
    if (this->m_k == 0) {
        std::uniform_int_distribution<> dist(1, 10);
        this->set_k(dist(gen));
    } else {
        // Ensure the dataset size is larger than or equal to the number of clusters
        if (datasize < this->m_k) {
            throw std::invalid_argument("Dataset size is smaller than the number of clusters.");
        }
    }

    // Container to hold indices of data points
    std::vector<int> indices(datasize);
    std::iota(indices.begin(), indices.end(), 0); // Fill indices with values from 0 to datasize - 1

    // Random sampling of indices without replacement
    std::vector<int> sampledIndices;
    std::sample(indices.begin(), indices.end(), std::back_inserter(sampledIndices), this->m_k, gen);

    int i = 0;
    // Create initial centroids using the sampled indices
    for (int idx : sampledIndices) {
        centroids.push_back(CentroidPoint<PT, PD>((this->m_data)[idx])); // Add a centroid from the selected point
        centroids[i].setID(i); // Assign a unique ID to the centroid
        i++;
    }

    this->exportedMesh(this->m_data, "Mesh");
    this->exportedMesh(centroids, "Centroids");
}

template class RandomCentroidInit<double, 2>;
template class RandomCentroidInit<double, 3>;