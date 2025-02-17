#include <vector>

#include <iostream>

#include "utils/CSVUtils.hpp"
#include "clustering/KMeans.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"

#define DIMENSION 2

using namespace std;

void printUsage() {
    std::cout << "Usage: ./program <csv_file> <num_clusters> <centroid_init_method> [k_init_method]\n";
    std::cout << "  <csv_file>             - Nome del file CSV (senza path)\n";
    std::cout << "  <num_clusters>         - Numero di cluster (0 se sconosciuto)\n";
    std::cout << "  <centroid_init_method> - Metodo di inizializzazione dei centroidi:\n";
    std::cout << "                           0: Random\n";
    std::cout << "                           1: Kernel Density Estimator\n";
    std::cout << "                           2: Most Distant\n";
    std::cout << "  [k_init_method]        - (Opzionale) Metodo per determinare k se num_clusters = 0:\n";
    std::cout << "                           0: Elbow Method\n";
    std::cout << "                           1: KDE\n";
    std::cout << "                           2: Silhouette\n";
    std::cout << "\nExample: ./program data.csv 3 1\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 4) {
            std::cerr << "Error: Not enough arguments!\n";
            printUsage();
            return 1;
        }

        std::string file_name = argv[1];
        std::string full_path = "../../resources/" + file_name;

        int num_clusters = std::stoi(argv[2]);
        int num_initialization_method = std::stoi(argv[3]);

        int kinitMethod = 0;  // Default
        if (num_clusters == 0) {
            if (argc < 5) {
                std::cerr << "Error: Missing k initialization method!\n";
                printUsage();
                return 1;
            }
            kinitMethod = std::stoi(argv[4]);
        }

        std::vector<Point<double, DIMENSION>> points;
        try {
            points = CSVUtils::readCSV<double, DIMENSION>(full_path);
        } catch (const std::exception &e) {
            std::cerr << "Failed to read CSV: " << e.what() << '\n';
            std::cerr << "Ensure the file exists at: " << full_path << '\n';
            return 1;
        }

        EuclideanMetric<double, DIMENSION> metric(points, 1e-4);
        KMeans<double, DIMENSION, EuclideanMetric<double, DIMENSION>> kmeans(num_clusters, 1e-4, &metric, num_initialization_method, kinitMethod);

        kmeans.fit();
        kmeans.print();

        return 0;
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
