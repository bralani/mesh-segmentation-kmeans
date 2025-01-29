#include <vector>
#include <benchmark/benchmark.h>
#include <iostream>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/GeodesicMetric.hpp"

using namespace std;

static const std::vector<std::string> mesh_files = {
    "../../resources/meshes/obj/125.obj",     // Piccolo
    "../../resources/meshes/obj/246.obj",    // Medio
    "../../resources/meshes/obj/103.obj",     // Grande
    "../../resources/meshes/obj/305.obj"       // Molto grande
};

static void BM_MeshSegmentation(benchmark::State& state) {
    int file_index = state.range(0);  
    if (file_index >= mesh_files.size()) return;

    std::string file_name = mesh_files[file_index];
    Mesh mesh(file_name);

    int n = mesh.getMeshFacesPoints().size();

    int num_clusters = 5;  
    int num_initialization_method = 0; 
    int num_k_init_method = 0; 

    for (auto _ : state) {
        MeshSegmentation<GeodesicHeatMetric<double, 3>> segmentation(
            &mesh, num_clusters, 1e-4, num_initialization_method, num_k_init_method
        );
        benchmark::DoNotOptimize(segmentation); 
    }

    state.SetComplexityN(n);
}

BENCHMARK(BM_MeshSegmentation)
    ->DenseRange(0, mesh_files.size() - 1, 1) 
    ->Complexity();  
BENCHMARK_MAIN();