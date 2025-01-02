cmake_minimum_required(VERSION 3.10)
project(KMeans)

# Add submodule for matplotlib-cpp (if this is still needed for your project)
add_subdirectory(include/matplotlib-cpp)

# Set the target architecture for macOS (arm64)
set(CMAKE_OSX_ARCHITECTURES arm64)

# Set C++ standard version (if required)
set(CMAKE_CXX_STANDARD 20)

# Include csv-parser as a submodule
set(CSV_PARSER_DIR "${CMAKE_SOURCE_DIR}/include/csv-parser")
include_directories(${CSV_PARSER_DIR}/single_include)

# Function to define an executable with common settings
function(add_kmeans_executable exe_name source_files)
    add_executable(${exe_name} ${source_files})
    target_link_libraries(${exe_name} PRIVATE matplotlib_cpp ${Python3_LIBRARIES} OpenMP::OpenMP_CXX)
    target_include_directories(${exe_name} PRIVATE include ${Python3_INCLUDE_DIRS})
    set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endfunction()

# Find Python3
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
message(STATUS "Python3 Executable: ${Python3_EXECUTABLE}")
message(STATUS "Python3 Library: ${Python3_LIBRARIES}")
message(STATUS "Python3 Include Directory: ${Python3_INCLUDE_DIRS}")

# OpenMP settings
find_package(OpenMP REQUIRED)
set(ENV{OMP_NESTED} TRUE)
set(ENV{OMP_NUM_THREADS} 8)

# Define executables
add_kmeans_executable(k_means "src/main.cpp;")
add_kmeans_executable(mesh_segmentation "src/segmentation.cpp;src/Mesh.cpp")
add_kmeans_executable(evaluation "src/evaluation.cpp;src/Mesh.cpp")
