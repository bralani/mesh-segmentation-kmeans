

#ifndef RANDOM_CENTROID_INIT_HPP
#define RANDOM_CENTROID_INIT_HPP

#include "geometry/point/CentroidPoint.hpp"
#include "geometry/point/Point.hpp"
#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"
#include <random>
#include <algorithm>
#include <stdexcept>

/**
 * @class RandomCentroidInit
 * @brief A class for initializing centroids randomly from the dataset.
 * 
 * This class implements the CentroidInitMethod interface and provides functionality
 * to select centroids randomly from the given dataset. It also includes methods to
 * calculate the centroid of a cluster.
 */
template <typename PT, std::size_t PD>
class RandomCentroidInit : public CentroidInitMethod<PT, PD> {
public:

    RandomCentroidInit(std::vector<Point<PT, PD>>& data, int k) : CentroidInitMethod<PT, PD>(data, k) {}

    RandomCentroidInit(std::vector<Point<PT, PD>>& data) : CentroidInitMethod<PT, PD>(data) {}

    void findCentroid(std::vector<CentroidPoint<PT, PD>>& centroids) override {
        std::random_device rd;
        std::mt19937 gen(rd());
        int datasize = this->m_data.size();

        if(this->m_k == 0){
            std::uniform_int_distribution<> dist(1, 10);
            this->set_k(dist(gen));
        }else{
            if ( datasize < this->m_k) {
                throw std::invalid_argument("Dataset size is smaller than the number of clusters.");
            }
        }

        // Contenitore per memorizzare gli indici casuali
        std::vector<int> indices( datasize );
        std::iota(indices.begin(), indices.end(), 0); // Riempie gli indici da 0 a data.size() - 1

        // Selezione casuale senza ripetizioni
        std::vector<int> sampledIndices;
        std::sample(indices.begin(), indices.end(), std::back_inserter(sampledIndices), this->m_k, gen);

        int i = 0;
        // Creazione dei centroidi iniziali
        for (int idx : sampledIndices)
        {
            centroids.push_back(CentroidPoint<PT, PD>((this->m_data)[idx]));
            centroids[i].setID(i);
            i++;
        }
    }

};

#endif 