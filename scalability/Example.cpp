#include <benchmark/benchmark.h>
#include <vector>
#include "../include/KDTree.hpp"
#include "../include/KDNode.hpp"
#include "../include/Point.hpp"
#include <random>

// Function to generate random points
template <typename PT, size_t PD>
std::vector<Point<PT, PD>> generateRandomPoints(int numPoints, double minValue, double maxValue) {
    std::vector<Point<PT, PD>> points;
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<> dis(minValue, maxValue);

    for (int i = 0; i < numPoints; ++i) {
        std::array<double, PD> coords;
        for (size_t d = 0; d < PD; ++d) {
            coords[d] = dis(gen);
        }
        points.emplace_back(coords);
    }

    return points;
}

static void BM_GenerateRandomPoints(benchmark::State& state) {
    int numPoints = state.range(0); // Number of points varies based on benchmark input
    size_t dimension = 3;             // Fixed dimension
    double minValue = -10.0, maxValue = 10.0;

    for (auto _ : state) {
        // Benchmark the random point generation
        auto points = generateRandomPoints<double, 3>(numPoints, minValue, maxValue);
        benchmark::DoNotOptimize(KdTree<double, 3>(points)); // Prevent compiler optimization
    }

    state.SetComplexityN(state.range(0)); // Report complexity based on the number of points
}

// Register the benchmark for different numbers of points
BENCHMARK(BM_GenerateRandomPoints)
    ->RangeMultiplier(2)    // Double the number of points for each test
    ->Range(8, 8 << 10)     // Test from 8 points up to 8192 points
    ->Complexity();         // Analyze time complexity

// Main function
BENCHMARK_MAIN();