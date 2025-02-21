#include <vector>
#include <benchmark/benchmark.h>
#include <iostream>
#include <omp.h>

#include "mesh_segmentation/MeshSegmentation.hpp"
#include "geometry/metrics/EuclideanMetric.hpp"
#include "geometry/metrics/GeodesicDijkstraMetric.hpp"

using namespace std;

static const std::vector<std::string> mesh_files = {
    std::string(ROOT_FOLDER) + "/resources/WeakTest/2k.obj",   
    std::string(ROOT_FOLDER) + "/resources/WeakTest/13k.obj",   
    std::string(ROOT_FOLDER) + "/resources/WeakTest/21k.obj",   
    std::string(ROOT_FOLDER) + "/resources/WeakTest/55k.obj",   
    std::string(ROOT_FOLDER) + "/resources/WeakTest/100k.obj",   
    std::string(ROOT_FOLDER) + "/resources/WeakTest/300k.obj",   
    std::string(ROOT_FOLDER) + "/resources/WeakTest/500k.obj",   
};

static void BM_MeshSegmentation(benchmark::State& state) {
    int num_file = state.range(0);  


    std::string file_name = mesh_files[num_file];
    Mesh mesh(file_name);

    int n = mesh.getMeshFacesPoints().size();

    int num_clusters = 5;  
    int num_initialization_method = 2; 
    int num_k_init_method = 0;

    omp_set_num_threads(8);

    for (auto _ : state) {
        MeshSegmentation<EuclideanMetric<double, 3>> segmentation(&mesh, num_clusters, 0.05, num_initialization_method, num_k_init_method);
        //segmentation.fit();
        benchmark::DoNotOptimize(segmentation); 
    }

    state.SetComplexityN(n);

}

BENCHMARK(BM_MeshSegmentation)
    ->DenseRange(1, 7, 1)                    
    ->Complexity();

BENCHMARK_MAIN();
