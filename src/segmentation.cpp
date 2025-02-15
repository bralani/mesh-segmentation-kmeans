#include <vector>

#include <iostream>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"

namespace plt = matplotlibcpp;

using namespace std;

int main()
{
  try
  {
    string file_name;
    cout << "Enter the name of the mesh file: ";
    cin >> file_name;
    file_name = std::string(ROOT_FOLDER) + "/resources/meshes/obj/" + file_name + ".obj";


    int num_clusters;
    cout << "Enter the number of clusters (parameter k, 0 if unknow): ";
    cin >> num_clusters;

    int num_initialization_method;
    std::cout << "Enter the number of initialization method for centroids \n (0: random, 1: kernel density estimator, 2: most distant, 3: Static KDE - 3D point )" << std::endl;
    cin >> num_initialization_method;

    Mesh mesh(file_name);


    int num_k_init_method=0;
    if(num_clusters == 0){
      std::cout<<"Enter the method for k initialization (0: elbow, 1: kde, 2: Silhoulette): ";
      cin >> num_k_init_method;
    }

    MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation(&mesh, num_clusters, 0.05, num_initialization_method, num_k_init_method) ;

    segmentation.fit();

    // Generate the output file path
    std::string output_file = file_name.substr(0, file_name.find_last_of('.')) + "_segmented.obj";

    // Export the mesh grouped by clusters
    mesh.exportToGroupedObj(output_file);

    std::cout << "Segmented mesh saved to: " << output_file << std::endl;

    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}

/*
void printUsage() {
    std::cout << "Usage: ./program <mesh_file> <num_clusters> <centroid_init_method> [k_init_method]\n";
    std::cout << "  <mesh_file>            - Nome del file della mesh (senza path)\n";
    std::cout << "  <num_clusters>         - Numero di cluster (0 se sconosciuto)\n";
    std::cout << "  <centroid_init_method> - Metodo di inizializzazione dei centroidi:\n";
    std::cout << "                           0: Random\n";
    std::cout << "                           1: Kernel Density Estimator\n";
    std::cout << "                           2: Most Distant\n";
    std::cout << "                           3: Static KDE - 3D point\n";
    std::cout << "  [k_init_method]        - (Opzionale) Metodo per determinare k se num_clusters = 0:\n";
    std::cout << "                           0: Elbow Method\n";
    std::cout << "                           1: KDE\n";
    std::cout << "                           2: Silhouette\n";
    std::cout << "\nExample: ./program mesh.obj 3 1\n";
}

int main(int argc, char* argv[]) {
    try {
        // Verifica che almeno i primi 3 argomenti siano passati
        if (argc < 4) {
            std::cerr << "Error: Not enough arguments!\n";
            printUsage();
            return 1;
        }

        std::string file_name = argv[1];
        std::string full_path = "../../resources/meshes/obj/" + file_name;

        int num_clusters = std::stoi(argv[2]);
        int num_initialization_method = std::stoi(argv[3]);

        int num_k_init_method = 0;  // Default
        if (num_clusters == 0) {
            if (argc < 5) {
                std::cerr << "Error: Missing k initialization method!\n";
                printUsage();
                return 1;
            }
            num_k_init_method = std::stoi(argv[4]);
        }

        // Creazione della mesh
        Mesh mesh(full_path);

        // Creazione della segmentazione della mesh
        MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation(
            &mesh, num_clusters, 1e-4, num_initialization_method, num_k_init_method
        );

        segmentation.fit();

        // Generazione del file di output
        std::string output_file = full_path.substr(0, full_path.find_last_of('.')) + "_segmented.obj";

        // Esporta la mesh segmentata
        mesh.exportToGroupedObj(output_file);

        std::cout << "Segmented mesh saved to: " << output_file << std::endl;

        return 0;
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
 */
