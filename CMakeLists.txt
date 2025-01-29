cmake_minimum_required(VERSION 3.10)
project(KMeans)

# Set the target architecture for macOS (arm64)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_CXX_STANDARD 20)

# CUDA settings
find_package(CUDA)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_50") # Set GPU architecture (adjust if necessary)

# Include csv-parser as a submodule
include_directories(SYSTEM third_party/csv-parser/single_include)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/eigen)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/libigl/include)
add_subdirectory(third_party/matplotlib-cpp)
add_subdirectory(src/model_renderer)

# BENCHMARKING 
set(BENCHMARK_ENABLE_TESTING OFF)  
add_subdirectory(third_party/benchmark)

# Function to define an executable with common settings
function(add_kmeans_executable exe_name source_files cuda_files)
    if(CUDA_FOUND AND MSVC)
        add_definitions(-DUSE_CUDA)
        cuda_add_executable(${exe_name} ${source_files} ${cuda_files})
        target_link_libraries(${exe_name} ${CUDA_LIBRARIES} matplotlib_cpp OpenMP::OpenMP_CXX)
    else()
        add_executable(${exe_name} ${source_files})
        target_link_libraries(${exe_name} matplotlib_cpp OpenMP::OpenMP_CXX)
    endif()

    target_compile_options(${exe_name} PRIVATE ${OpenMP_CXX_FLAGS})
    target_include_directories(${exe_name} PRIVATE include third_party)
    set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set_target_properties(${exe_name} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endfunction()

# OpenMP settings
find_package(OpenMP REQUIRED)
set(ENV{OMP_NESTED} TRUE)
set(ENV{OMP_NUM_THREADS} 8)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp:llvm /openmp:experimental")
endif()

file(GLOB_RECURSE SOURCES 
    "src/geometry/*.cpp" 
    "src/clustering/*.cpp" 
)

# Define executables
add_kmeans_executable(k_means "src/main.cpp;${SOURCES}" "include/clustering/kmeans.cu")
add_kmeans_executable(mesh_segmentation "src/segmentation.cpp;${SOURCES}" "include/clustering/kmeans.cu")
add_kmeans_executable(evaluation "src/evaluation.cpp;${SOURCES}" "include/clustering/kmeans.cu")

add_kmeans_executable(kmeans_benchmark "src/benchmark.cpp;${SOURCES}" "include/clustering/kmeans.cu")
target_link_libraries(kmeans_benchmark benchmark::benchmark)

