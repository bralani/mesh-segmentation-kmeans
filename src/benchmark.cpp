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

    int num_threads = state.range(0);

    std::string file_name = "../../resources/meshes/obj/125.obj";
    Mesh mesh(file_name);

    int n = mesh.getMeshFacesPoints().size();

    int num_clusters = 5;  
    int num_initialization_method = 0; 
    int num_k_init_method = 0;

    omp_set_num_threads(num_threads);  // Set number of threads for OpenMP

    for (auto _ : state) {
        #pragma omp parallel
        {
            MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation(
                &mesh, num_clusters, 1e-4, num_initialization_method, num_k_init_method);
            benchmark::DoNotOptimize(segmentation); 
        }
    }
    state.SetComplexityN(n);
}

BENCHMARK(BM_MeshSegmentation)
    ->ArgsProduct({benchmark::CreateDenseRange(1, 8, 1)}) // File indices
    ->Complexity();

BENCHMARK_MAIN();

