#include <vector>
#include <benchmark/benchmark.h>
#include <iostream>
#include <omp.h>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"

using namespace std;

static const std::vector<std::string> mesh_files = {
    "../../resources/meshes/obj/125.obj",   // Smallest file
    "../../resources/meshes/obj/246.obj",   // 
    "../../resources/meshes/obj/103.obj",   // 
    "../../resources/meshes/obj/305.obj"    // Biggest file 
};

static void BM_MeshSegmentation(benchmark::State& state) {
    int file_index = state.range(0);
    int num_threads = state.range(1);  // Number of threads to test

    if (file_index >= mesh_files.size()) return;

    std::string file_name = mesh_files[file_index];
    Mesh mesh(file_name);

    int n = mesh.getMeshFacesPoints().size();

    int num_clusters = 5;  
    int num_initialization_method = 0; 
    int num_k_init_method = 0;

    omp_set_num_threads(num_threads);  // Set number of threads for OpenMP

    for (auto _ : state) {
        MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation( &mesh, num_clusters, 1e-4, num_initialization_method, num_k_init_method);
        benchmark::DoNotOptimize(segmentation); 
    }

    state.SetComplexityN(n);

    // Stampa a video con informazioni dettagliate
    if (state.thread_index() == 0) {  
        std::cout << "File: " << file_name
                  << ", Threads: " << num_threads
                  << ", Mesh Faces Points: " << n
                  << std::endl;
    }
}

BENCHMARK(BM_MeshSegmentation)
    ->DenseRange(0, mesh_files.size() - 1, 1)   // Iterate over file indices
    ->DenseRange(1, 8, 1)                       // Iterate over 1, 2, ..., 8 threads
    ->Complexity();

BENCHMARK_MAIN();
