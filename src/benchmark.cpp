#include <vector>
#include <benchmark/benchmark.h>
#include <iostream>
#include <omp.h>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"

using namespace std;

static const std::vector<std::string> mesh_files = {
    std::string(ROOT_FOLDER) + "/resources/meshes/obj/125.obj",   // Smallest file
    std::string(ROOT_FOLDER) + "/resources/meshes/obj/246.obj",   // 
    std::string(ROOT_FOLDER) + "/resources/meshes/obj/103.obj",   // 
    std::string(ROOT_FOLDER) + "/resources/meshes/obj/305.obj"    // Biggest file 
};

static void BM_MeshSegmentation(benchmark::State& state) {
    int num_threads = state.range(0);  // Number of threads to test


    std::string file_name = mesh_files[2];
    Mesh mesh(file_name);

    int n = mesh.getMeshFacesPoints().size();

    int num_clusters = 5;  
    int num_initialization_method = 2; 
    int num_k_init_method = 0;

    omp_set_num_threads(num_threads);  // Set number of threads for OpenMP

    for (auto _ : state) {
        MeshSegmentation<GeodesicMetric<double, 3>> segmentation(&mesh, num_clusters, 0.05, num_initialization_method, num_k_init_method);
        segmentation.fit();
        benchmark::DoNotOptimize(segmentation); 
    }

    state.SetComplexityN(n);

}

BENCHMARK(BM_MeshSegmentation)
    ->DenseRange(1, 8, 1)                       // Iterate over 1, 2, ..., 8 threads
    ->Complexity();

BENCHMARK_MAIN();
